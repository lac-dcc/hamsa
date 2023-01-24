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
    kernel->parent = root;
    this->loopKernels[id] = kernel;
  }

  this->handleForInit(fstmt->getInit(), kernel);
  this->handleForCond(fstmt->getCond(), kernel);
  this->handleForInc(fstmt->getInc(), kernel);
  this->handleForBody(fstmt->getBody(), kernel);

  if (kernel->parent != nullptr)
    kernel->parent->children.insert(kernel);

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
      this->ifStmtParents[id] = kernel->child;
    }

    if (firstCall) {
      if (auto* nestedFor = dyn_cast<ForStmt>(child)) {
        LoopKernel* childKernel = new LoopKernel(nestedFor->getID(*this->context));
        childKernel->parent = kernel->child;
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
  CondKernel* cond = nullptr;

  auto thenBody = ifstmt->getThen();
  this->traverseIfBody(thenBody, id, cond);

  if (cond != nullptr) {
    for (auto child : thenBody->children()) {
      if (auto foundIf = dyn_cast<IfStmt>(child))
        this->ifStmtParents[foundIf->getID(*this->context)] = cond->thenChild;
    }
  }

  if (ifstmt->hasElseStorage()) {
    auto elseBody = ifstmt->getElse();
    this->traverseIfBody(elseBody, id, cond, true);
    if (cond != nullptr) {
      for (auto child : elseBody->children()) {
        if (auto foundIf = dyn_cast<IfStmt>(child))
          this->ifStmtParents[foundIf->getID(*this->context)] = cond->elseChild;
      }
    }
  }

  if (cond != nullptr && cond->parent != nullptr) {
    cond->parent->children.insert(cond);
    cond->condition = ifstmt->getCond();
  }

  return true;
}

void LoopInfoVisitor::handleIfBody(clang::Stmt* body, CondKernel* kernel) {}

void LoopInfoVisitor::traverseIfBody(clang::Stmt* node, int64_t& ifstmtId, CondKernel*& cond, bool isElse) {
  for (auto* child : node->children()) {
    if (!child)
      continue;

    if (auto forStmt = dyn_cast<ForStmt>(child)) {
      if (cond == nullptr) {
        cond = new CondKernel(ifstmtId);
        if (this->ifStmtParents.find(ifstmtId) != this->ifStmtParents.end()) {
          cond->parent = this->ifStmtParents[ifstmtId];
        } else {
          cond->parent = this->root;
        }
      }
      auto id = forStmt->getID(*this->context);
      LoopKernel* loop;
      if (this->loopKernels.find(id) != this->loopKernels.end()) {
        loop = this->loopKernels[id];
      } else {
        loop = new LoopKernel(id);
        this->loopKernels[id] = loop;
      }

      if (isElse) {
        loop->parent = cond->elseChild;
      } else {
        loop->parent = cond->thenChild;
      }
    }
    this->traverseIfBody(child, ifstmtId, cond, isElse);
  }
}

void LoopInfoConsumer::HandleTranslationUnit(ASTContext& Context) {
  visitor.TraverseDecl(Context.getTranslationUnitDecl());

  if (this->outputFormat == "txt" || this->outputFormat == "TXT") {
    ComplexityKernelVisitor complexityVisitor(&Context);
    complexityVisitor.visit(visitor.root);

    TxtPrinter printer;
    printer.gen_out(visitor.root, Context, this->outputFile);
  } else if (this->outputFormat == "dot" || this->outputFormat == "DOT") {
    DotPrinter printer;
    printer.gen_out(visitor.root, Context, this->outputFile);
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