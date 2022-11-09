#include "ForInfoTool.hpp"
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/Lexer.h"

bool FindForCondVisitor::VisitForStmt(ForStmt* fstmt, bool nested) {
  std::string induction, valBegin, valEnd, increment;
  this->handleForInit(fstmt->getInit(), induction, valBegin);
  this->handleForCond(fstmt->getCond(), valEnd);
  this->handleForInc(fstmt->getInc(), increment);

  outs() << induction << ", <" << valBegin << ", " << valEnd << ", " << increment << "> ";
  outs() << "{ \n";
  this->handleForBody(fstmt->getBody(), nested);
  outs() << "}\n";

  // Removing VarDecl from inputs
  if (this->bodyDeclarations.size() != 0 && !nested) {
    for (auto* bDecl : this->bodyDeclarations) {
      for (auto* input : this->inputsBuffer) {
        if (input->getNameAsString() == bDecl->getNameAsString())
          this->inputsBuffer.erase(input);
      }
    }
  }

  if (this->inputsBuffer.size() != 0 && !nested) {
    bool isFirst = true;
    outs() << "[";
    for (auto* input : this->inputsBuffer) {
      outs() << (isFirst ? isFirst = false, "" : ", ") << input->getNameAsString();
    }
    outs() << "]\n";
  }

  if (!nested) {
    this->inputsBuffer.clear();
    this->bodyDeclarations.clear();
  }

  return true;
}

void FindForCondVisitor::traverseForBody(Stmt* node, bool nested, bool firstCall) {
  for (auto* child : node->children()) {
    if (!child)
      continue;

    if (auto* ref = dyn_cast<DeclRefExpr>(child))
      this->inputsBuffer.insert(ref->getDecl());

    if (!nested) {
      if (auto* declStmt = dyn_cast<DeclStmt>(child)) {
        for (auto* decl : declStmt->decls()) {
          if (auto* varDecl = dyn_cast<VarDecl>(decl))
            this->bodyDeclarations.push_back(varDecl);
        }
      }
    }

    if (firstCall) {
      if (auto* nestedFor = dyn_cast<ForStmt>(child))
        VisitForStmt(nestedFor, true);
    }

    this->traverseForBody(child, nested, false);
  }
}

void FindForCondVisitor::traverseExpr(Stmt* node) {
  for (auto* child : node->children()) {
    if (!child)
      continue;

    if (auto* ref = dyn_cast<DeclRefExpr>(child))
      this->inputsBuffer.insert(ref->getDecl());

    this->traverseExpr(child);
  }
}

void FindForCondVisitor::handleForInit(Stmt* init, std::string& induc, std::string& valBegin) {
  if (!init)
    return;

  // Initialization as assignment expression
  if (auto* assign = dyn_cast<BinaryOperator>(init)) {
    if (assign->isAssignmentOp()) {
      if (auto* initVar = dyn_cast<VarDecl>(assign->getLHS()->getReferencedDeclOfCallee()))
        induc = initVar->getNameAsString();

      // Initialization with RHS as an integer
      if (auto* initValInt = dyn_cast<IntegerLiteral>(assign->getRHS())) {
        valBegin = std::to_string((int)initValInt->getValue().roundToDouble());
      } else if (auto* initDeclRef = dyn_cast<VarDecl>(assign->getRHS()->getReferencedDeclOfCallee())) {
        this->inputsBuffer.insert(initDeclRef);
        valBegin = initDeclRef->getNameAsString();
      }
    }
  }
  // Initialization with a var declaration
  else if (auto* varDeclStmt = dyn_cast<DeclStmt>(init)) {
    if (auto* valDecl = dyn_cast<VarDecl>(varDeclStmt->getSingleDecl())) {
      induc = valDecl->getNameAsString();
      this->bodyDeclarations.push_back(valDecl);

      // Initialization with an integer
      if (auto* varDeclInt = dyn_cast<IntegerLiteral>(valDecl->getInit()))
        valBegin = std::to_string((int)varDeclInt->getValue().roundToDouble());
      // Initialization with another variable
      else if (auto* varDeclRef =
                   dyn_cast<VarDecl>(valDecl->getInit()->IgnoreImpCasts()->getReferencedDeclOfCallee())) {
        this->inputsBuffer.insert(varDeclRef);
        valBegin = varDeclRef->getNameAsString();
      } else if (auto* varDeclExpr = dyn_cast<Expr>(valDecl->getInit())) {
        CharSourceRange srcRange = CharSourceRange::getTokenRange(varDeclExpr->getSourceRange());
        SourceManager& srcManager = this->Context->getSourceManager();
        const LangOptions& langOpts = this->Context->getLangOpts();
        valBegin = Lexer::getSourceText(srcRange, srcManager, langOpts).str();
        this->traverseExpr(varDeclExpr);
      }
    }
  }
}

void FindForCondVisitor::handleForCond(Expr* cond, std::string& valEnd) {
  if (!cond)
    return;

  if (auto* bo = dyn_cast<BinaryOperator>(cond)) {
    auto* boolRHS = bo->getRHS();
    auto* boolLHS = bo->getLHS();
    if (auto* condvarL = dyn_cast<VarDecl>(boolLHS->getReferencedDeclOfCallee())) {
      // For ForCondExpr like "i < n"
      if (auto* condvarR = dyn_cast<VarDecl>(boolRHS->getReferencedDeclOfCallee())) {
        this->inputsBuffer.insert(condvarR);
        valEnd = condvarR->getNameAsString();
      }
      // For ForCondExpr like "i > 10"
      else if (auto* condvalR = dyn_cast<IntegerLiteral>(boolRHS)) {
        valEnd = std::to_string((int)condvalR->getValue().roundToDouble());
      }
    }
  }
}

void FindForCondVisitor::handleForInc(Expr* inc, std::string& increment) {
  if (!inc)
    return;

  if (auto* unaryOp = dyn_cast<UnaryOperator>(inc)) {
    if (unaryOp->isIncrementDecrementOp()) {
      if (unaryOp->isDecrementOp())
        increment = "-1";
      else
        increment = "1";
    }
  }
}

void FindForCondVisitor::handleForBody(Stmt* body, bool nested) {
  if (!body)
    return;

  if (auto* bodyStmt = dyn_cast<CompoundStmt>(body))
    this->traverseForBody(bodyStmt, nested);
}