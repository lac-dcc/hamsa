#include "ForInfoTool.hpp"

bool FindForCondVisitor::VisitForStmt(ForStmt* fstmt, bool nested) {
  std::string induction, valBegin, valEnd, increment;
  handleForInit(fstmt->getInit(), induction, valBegin);
  handleForCond(fstmt->getCond(), valEnd);
  handleForInc(fstmt->getInc(), increment);

  outs() << induction << ", <" << valBegin << ", " << valEnd << ", " << increment << "> ";
  outs() << "{ \n";
  handleForBody(fstmt->getBody(), nested);
  outs() << "}\n";

  // Removing VarDecl from inputs
  if (bodyDeclarations.size() != 0 && !nested) {
    for (auto* bDecl : bodyDeclarations) {
      for (auto* input : inputsBuffer) {
        if (input->getNameAsString() == bDecl->getNameAsString())
          inputsBuffer.erase(input);
      }
    }
  }

  if (inputsBuffer.size() != 0 && !nested) {
    bool isFirst = true;
    outs() << "[";
    for (auto* input : inputsBuffer) {
      outs() << (isFirst ? isFirst = false, "" : ", ") << input->getNameAsString();
    }
    outs() << "]\n";
  }
  if (!nested) {
    inputsBuffer.clear();
    bodyDeclarations.clear();
  }

  return true;
}

void FindForCondVisitor::DFS(Stmt* node, bool nested, bool firstCall) {
  for (auto it = node->child_begin(), itEnd = node->child_end(); it != itEnd; ++it) {
    if (*it) {
      if (auto* ref = dyn_cast<DeclRefExpr>(*it))
        inputsBuffer.insert(ref->getDecl());

      if (!nested) {
        if (auto* decl = dyn_cast<DeclStmt>(*it)) {
          for (auto declIt = decl->decl_begin(), declEnd = decl->decl_end(); declIt != declEnd; ++declIt) {
            if (auto* varDecl = dyn_cast<VarDecl>(*declIt))
              bodyDeclarations.push_back(varDecl);
          }
        }
      }

      if (firstCall) {
        if (auto* nestedFor = dyn_cast<ForStmt>(*it))
          VisitForStmt(nestedFor, true);
      }

      DFS(*it, nested, false);
    }
  }
}

void FindForCondVisitor::handleForInit(Stmt* init, std::string& induc, std::string& valBegin) {
  if (init) {
    // Initialization as assignment expression
    if (auto* assign = dyn_cast<BinaryOperator>(init)) {
      if (assign->isAssignmentOp()) {
        if (auto* initVar = dyn_cast<VarDecl>(assign->getLHS()->getReferencedDeclOfCallee()))
          induc = initVar->getNameAsString();

        // Initialization with RHS as an integer
        if (auto* initValInt = dyn_cast<IntegerLiteral>(assign->getRHS())) {
          valBegin = std::to_string((int)initValInt->getValue().roundToDouble());
        } else if (auto* initDeclRef = dyn_cast<VarDecl>(assign->getRHS()->getReferencedDeclOfCallee())) {
          inputsBuffer.insert(initDeclRef);
          valBegin = initDeclRef->getNameAsString();
        }
      }
    }
    // Initialization with a var declaration
    else if (auto* varDeclStmt = dyn_cast<DeclStmt>(init)) {
      if (auto* valDecl = dyn_cast<VarDecl>(varDeclStmt->getSingleDecl())) {
        induc = valDecl->getNameAsString();
        bodyDeclarations.push_back(valDecl);

        // Initialization with an integer
        if (auto* varDeclInt = dyn_cast<IntegerLiteral>(valDecl->getInit()))
          valBegin = std::to_string((int)varDeclInt->getValue().roundToDouble());
        // Initialization with another variable
        else if (auto* varDeclRef =
                     dyn_cast<VarDecl>(valDecl->getInit()->IgnoreImpCasts()->getReferencedDeclOfCallee())) {
          inputsBuffer.insert(varDeclRef);
          valBegin = varDeclRef->getNameAsString();
        }
      }
    }
  }
}

void FindForCondVisitor::handleForCond(Expr* cond, std::string& valEnd) {
  if (cond) {
    if (auto* bo = dyn_cast<BinaryOperator>(cond)) {
      auto* boolRHS = bo->getRHS();
      auto* boolLHS = bo->getLHS();
      if (auto* condvarL = dyn_cast<VarDecl>(boolLHS->getReferencedDeclOfCallee())) {
        // For ForCondExpr like "i < n"
        if (auto* condvarR = dyn_cast<VarDecl>(boolRHS->getReferencedDeclOfCallee())) {
          inputsBuffer.insert(condvarR);
          valEnd = condvarR->getNameAsString();
        }
        // For ForCondExpr like "i > 10"
        else if (auto* condvalR = dyn_cast<IntegerLiteral>(boolRHS)) {
          valEnd = std::to_string((int)condvalR->getValue().roundToDouble());
        }
      }
    }
  }
}

void FindForCondVisitor::handleForInc(Expr* inc, std::string& increment) {
  if (inc) {
    if (auto* unaryOp = dyn_cast<UnaryOperator>(inc)) {
      if (unaryOp->isIncrementDecrementOp()) {
        if (unaryOp->isDecrementOp())
          increment = "-1";
        else
          increment = "1";
      }
    }
  }
}

void FindForCondVisitor::handleForBody(Stmt* body, bool nested) {
  if (body) {
    if (auto* bodyStmt = dyn_cast<CompoundStmt>(body))
      DFS(bodyStmt, nested);
  }
}