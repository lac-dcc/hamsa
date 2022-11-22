#include "LoopInfoTool.hpp"
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/Lexer.h"
#include "Printer.hpp"

bool LoopInfoVisitor::VisitForStmt(ForStmt* fstmt, Kernel* parent) {
  Kernel* kernel;
  int64_t id = fstmt->getID(*this->context);
  if (kernels.find(id) != kernels.end())
    kernel = kernels[id];
  else {
    kernel = new Kernel;
    kernels.insert(std::make_pair(id, kernel));

    if (parent != nullptr) {
      kernel->parent = parent;
      return true;
    }
  }

  this->handleForInit(fstmt->getInit(), kernel);
  this->handleForCond(fstmt->getCond(), kernel);
  this->handleForInc(fstmt->getInc(), kernel);
  this->handleForBody(fstmt->getBody(), kernel);

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

void LoopInfoVisitor::traverseForBody(Stmt* node, Kernel* kernel, bool firstCall) {
  for (auto* child : node->children()) {
    if (!child)
      continue;

    if (auto* ref = dyn_cast<DeclRefExpr>(child)) {
      if (!ref->getDecl()->isFunctionOrFunctionTemplate())
        this->inputsBuffer.insert(ref->getDecl());
    }

    if (auto* declStmt = dyn_cast<DeclStmt>(child)) {
      for (auto* decl : declStmt->decls()) {
        if (auto* varDecl = dyn_cast<VarDecl>(decl))
          this->bodyDeclarations.insert(std::make_pair(varDecl, varDecl->getNameAsString()));
      }
    }

    if (firstCall) {
      if (auto* nestedFor = dyn_cast<ForStmt>(child))
        VisitForStmt(nestedFor, kernel);
    }

    this->traverseForBody(child, kernel, false);
  }
}

void LoopInfoVisitor::traverseExpr(Stmt* node) {
  for (auto* child : node->children()) {
    if (!child)
      continue;

    if (auto* ref = dyn_cast<DeclRefExpr>(child)) {
      if (!ref->getDecl()->isFunctionOrFunctionTemplate())
        this->inputsBuffer.insert(ref->getDecl());
    }

    this->traverseExpr(child);
  }
}

void LoopInfoVisitor::handleForInit(Stmt* init, Kernel* kernel) {
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
        this->traverseExpr(initExpr);
    }
  }
  // Initialization with a var declaration
  else if (auto* varDeclStmt = dyn_cast<DeclStmt>(init)) {
    if (auto* valDecl = dyn_cast<VarDecl>(varDeclStmt->getSingleDecl())) {
      kernel->induc = valDecl;
      this->bodyDeclarations.insert(std::make_pair(valDecl, valDecl->getNameAsString()));

      kernel->init = valDecl->getInit();

      // Initialization as another variable
      if (auto* varDeclRef = dyn_cast<VarDecl>(valDecl->getInit()->IgnoreImpCasts()->getReferencedDeclOfCallee()))
        kernel->inputs.insert(varDeclRef);
      // Initialization as an expression
      else if (auto* varDeclExpr = dyn_cast<Expr>(valDecl->getInit()))
        this->traverseExpr(varDeclExpr);
    }
  }
}

void LoopInfoVisitor::handleForCond(Expr* cond, Kernel* kernel) {
  if (!cond)
    return;

  if (auto* bo = dyn_cast<BinaryOperator>(cond)) {
    kernel->limit = bo->getRHS();

    if (auto* condvarR = dyn_cast<VarDecl>(bo->getRHS()->getReferencedDeclOfCallee()))
      kernel->inputs.insert(condvarR);
    else if(auto* condvalR = dyn_cast<Expr>(bo->getRHS())) 
      this->traverseExpr(condvalR);
  }
}

void LoopInfoVisitor::handleForInc(Expr* inc, Kernel* kernel) {
  if (!inc)
    return;

  kernel->inc = inc;
}

void LoopInfoVisitor::handleForBody(Stmt* body, Kernel* kernel) {
  if (!body)
    return;

  if (auto* bodyStmt = dyn_cast<CompoundStmt>(body))
    this->traverseForBody(bodyStmt, kernel);
}

std::string LoopInfoVisitor::getExprAsString(Expr* expr) {
  CharSourceRange srcRange = CharSourceRange::getTokenRange(expr->getSourceRange());
  SourceManager& srcManager = this->context->getSourceManager();
  const LangOptions& langOpts = this->context->getLangOpts();
  return Lexer::getSourceText(srcRange, srcManager, langOpts).str();
}

DenseMap<int64_t, Kernel*> LoopInfoVisitor::getKernels() {
    return kernels;
}

void LoopInfoConsumer::HandleTranslationUnit(ASTContext& Context) {
    visitor.TraverseDecl(Context.getTranslationUnitDecl());
    TextPrinter p;
    p.gen_out(visitor.getKernels(), Context);
}
