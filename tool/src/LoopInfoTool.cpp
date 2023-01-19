#include "Complexity.hpp"
#include "KernelVisitor.hpp"
#include "LoopInfoTool.hpp"
#include "Printer.hpp"

using namespace clang;
using namespace llvm;

bool LoopInfoVisitor::VisitForStmt(ForStmt* fstmt) {
  LoopKernel* kernel;
  int64_t id = fstmt->getID(*this->context);
  if (this->loopKernels.find(id) != this->loopKernels.end()) {
    kernel = this->loopKernels[id];
  } else {
    kernel = new LoopKernel(id);
    root->children.insert(kernel);
    kernel->parent = root;
    this->loopKernels[id] = kernel;
  }

  this->handleForInit(fstmt->getInit(), kernel);
  this->handleForCond(fstmt->getCond(), kernel);
  this->handleForInc(fstmt->getInc(), kernel);
  this->handleForBody(fstmt->getBody(), kernel);
  kernel->begin = fstmt->getSourceRange().getBegin();

  // Removing VarDecl from inputs
  if (!this->bodyDeclarations.empty()) {
    for (auto* input : kernel->inputs) {
      if (bodyDeclarations.find(input) != bodyDeclarations.end())
        kernel->inputs.erase(input);
    }
  }

  this->bodyDeclarations.clear();

  return true;
}

void LoopInfoVisitor::traverseForBody(Stmt* node, LoopKernel* kernel, bool firstCall) {
  for (auto* child : node->children()) {
    if (!child)
      continue;

    if (auto* ref = dyn_cast<DeclRefExpr>(child)) {
      if (!ref->getDecl()->isFunctionOrFunctionTemplate())
        kernel->inputs.insert(ref->getDecl());
    }

    if (auto* declStmt = dyn_cast<DeclStmt>(child)) {
      for (auto* decl : declStmt->decls()) {
        if (auto* varDecl = dyn_cast<VarDecl>(decl))
          this->bodyDeclarations[varDecl] = varDecl->getNameAsString();
      }
    }

    if (auto* ifstmt = dyn_cast<IfStmt>(child)) {
      auto id = ifstmt->getID(*this->context);
      auto cond = new CondKernel(id);
      this->condKernels[id] = cond;
      cond->parent = kernel->child;
      kernel->child->children.insert(cond);
    }

    if (firstCall) {
      if (auto* nestedFor = dyn_cast<ForStmt>(child)) {
        LoopKernel* childKernel = new LoopKernel(nestedFor->getID(*this->context));
        childKernel->parent = kernel->child;
        kernel->child->children.insert(childKernel);
        this->loopKernels[childKernel->id] = childKernel;
      }
    }

    this->traverseForBody(child, kernel, false);
  }
}

void LoopInfoVisitor::traverseExpr(Stmt* node, LoopKernel* kernel) {
  for (auto* child : node->children()) {
    if (!child)
      continue;

    if (auto* ref = dyn_cast<DeclRefExpr>(child)) {
      if (!ref->getDecl()->isFunctionOrFunctionTemplate())
        kernel->inputs.insert(ref->getDecl());
    }

    this->traverseExpr(child, kernel);
  }
}

void LoopInfoVisitor::handleForInit(Stmt* init, LoopKernel* kernel) {
  if (!init)
    return;

  // Initialization as assignment expression
  if (auto* assign = dyn_cast<BinaryOperator>(init)) {
    if (assign->isAssignmentOp()) {
      if (auto* initVar = dyn_cast<VarDecl>(assign->getLHS()->getReferencedDeclOfCallee()))
        kernel->induc = initVar;

      kernel->init = assign->getRHS();

      // Initialization with RHS as another variable
      if (auto* initDeclRef = dyn_cast<VarDecl>(assign->getRHS()->getReferencedDeclOfCallee()))
        kernel->inputs.insert(initDeclRef);
      // Initialization with RHS as an expression
      else if (auto* initExpr = dyn_cast<Expr>(assign->getRHS()))
        this->traverseExpr(initExpr, kernel);
    }
  }
  // Initialization with a var declaration
  else if (auto* varDeclStmt = dyn_cast<DeclStmt>(init)) {
    if (auto* valDecl = dyn_cast<VarDecl>(varDeclStmt->getSingleDecl())) {
      kernel->induc = valDecl;
      this->bodyDeclarations[valDecl] = valDecl->getNameAsString();

      kernel->init = valDecl->getInit();

      // Initialization as another variable
      if (auto* varDeclRef = dyn_cast<VarDecl>(valDecl->getInit()->IgnoreImpCasts()->getReferencedDeclOfCallee()))
        kernel->inputs.insert(varDeclRef);
      // Initialization as an expression
      else if (auto* varDeclExpr = dyn_cast<Expr>(valDecl->getInit()))
        this->traverseExpr(varDeclExpr, kernel);
    }
  }
}

void LoopInfoVisitor::handleForCond(Expr* cond, LoopKernel* kernel) {
  if (!cond)
    return;

  if (auto* bo = dyn_cast<BinaryOperator>(cond)) {
    kernel->limit = bo->getRHS();

    if (auto* condvarR = dyn_cast<VarDecl>(bo->getRHS()->getReferencedDeclOfCallee()))
      kernel->inputs.insert(condvarR);
    else if (auto* condvalR = dyn_cast<Expr>(bo->getRHS()))
      this->traverseExpr(condvalR, kernel);
  }
}

void LoopInfoVisitor::handleForInc(Expr* inc, LoopKernel* kernel) {
  if (!inc)
    return;

  kernel->inc = inc;
}

void LoopInfoVisitor::handleForBody(Stmt* body, LoopKernel* kernel) {
  if (!body)
    return;

  if (auto* bodyStmt = dyn_cast<CompoundStmt>(body))
    this->traverseForBody(bodyStmt, kernel);
}

bool LoopInfoVisitor::VisitIfStmt(IfStmt* ifstmt) {
  auto id = ifstmt->getID(*this->context);
  CondKernel* cond;
  if (this->condKernels.find(id) != this->condKernels.end()) {
    cond = condKernels[id];
  } else {
    auto cond = new CondKernel(id);
  }
  auto thenBody = ifstmt->getThen();
  bool found = false;

  for (auto child : thenBody->children()) {
    if (auto foundFor = dyn_cast<ForStmt>(child)) {
      found = true;
      auto id = foundFor->getID(*this->context);
      if (this->loopKernels.find(id) == this->loopKernels.end()) {
        auto loop = new LoopKernel(id);
        this->loopKernels[id] = loop;
        loop->parent = cond->thenChild;
        cond->thenChild->children.insert(loop);
      }
    }
  }

  if (ifstmt->hasElseStorage()) {
    auto elseBody = ifstmt->getElse();
    for (auto child : elseBody->children()) {
      if (auto foundFor = dyn_cast<ForStmt>(child)) {
        found = true;
        auto id = foundFor->getID(*this->context);
        if (this->loopKernels.find(id) == this->loopKernels.end()) {
          auto loop = new LoopKernel(id);
          this->loopKernels.insert(std::make_pair(id, loop));
          cond->elseChild->children.insert(loop);
        }
      }
    }
  }

  if (found && this->condKernels.find(id) == this->condKernels.end()) {
    this->condKernels[cond->id] = cond;
    cond->parent = this->root;
    this->root->children.insert(cond);
  }
  return true;
}

DenseMap<int64_t, LoopKernel*> LoopInfoVisitor::getKernels() { return this->loopKernels; }

void LoopInfoConsumer::HandleTranslationUnit(ASTContext& Context) {
  visitor.TraverseDecl(Context.getTranslationUnitDecl());

  if (this->outputFormat == "txt" || this->outputFormat == "TXT") {
    ComplexityKernelVisitor complexityVisitor(&Context);
    complexityVisitor.visit(visitor.root);

    TextPrinter printer;
    printer.gen_out(visitor.getKernels(), visitor.root, Context, this->outputFile);
  } else if (this->outputFormat == "dot" || this->outputFormat == "DOT") {
    DOTPrinter printer;
    printer.gen_out(visitor.getKernels(), visitor.root, Context, this->outputFile);
  }
}

bool LoopInfoAction::ParseArgs(const CompilerInstance& Compiler, const std::vector<std::string>& args) {
  for (size_t i = 0, end = args.size(); i < end; ++i) {
    DiagnosticsEngine& diagnostics = Compiler.getDiagnostics();
    if (args[i] == "-output-format") {
      if (i + 1 >= end) {
        diagnostics.Report(diagnostics.getCustomDiagID(DiagnosticsEngine::Error, "Missing -output argument"));
        return false;
      }

      ++i;
      this->outputFormat = args[i];
    } else if (args[i] == "-output-file") {
      if (i + 1 >= end) {
        diagnostics.Report(diagnostics.getCustomDiagID(DiagnosticsEngine::Error, "Missing -output-file argument"));
        return false;
      }

      ++i;
      this->outputFile = args[i];
    } else if (args[i] == "-help") {
      errs() << "--- Hamsa plugin ---\n";
      errs() << "Arguments:\n";
      errs() << " -output-format   format\n";
      errs() << " -output-file     fileName\n";
    } else {
      unsigned DiagID = diagnostics.getCustomDiagID(DiagnosticsEngine::Error, "Invalid argument '%0'");
      diagnostics.Report(DiagID) << args[i];

      return false;
    }
  }

  return true;
}