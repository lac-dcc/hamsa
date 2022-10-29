#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/ADT/SmallSet.h"

using namespace clang;
using namespace clang::tooling;
using namespace llvm;

static llvm::cl::OptionCategory MatcherCategory("matcher options");

class FindForCondVisitor : public RecursiveASTVisitor<FindForCondVisitor> {
public:
  explicit FindForCondVisitor(ASTContext* Context) : Context(Context) {}

  bool VisitForStmt(ForStmt* fstmt, bool nested = false) {

    std::string induction, valBegin, valEnd, increment;
    handleForInit(fstmt->getInit(), induction, valBegin);
    handleForCond(fstmt->getCond(), valEnd);
    handleForInc(fstmt->getInc(), increment);

    outs() << induction << ", <" << valBegin << ", " << valEnd << ", " << increment << "> ";
    outs() << "{ \n";
    handleForBody(fstmt->getBody());
    outs() << "}\n";

    if (inputsBuffer.size() != 0 && nested == false) {
      bool isFirst = true;
      outs() << "[";
      for (auto input : inputsBuffer) {
        outs() << (isFirst ? isFirst = false, "" : ", ") << input->getNameAsString();
      }
      outs() << "]\n";
    }

    if (nested == false && inputsBuffer.size())
      inputsBuffer.clear();
    return true;
  }

private:
  ASTContext* Context;

  // This container is used to store the for's inputs
  SmallSet<VarDecl*, 6> inputsBuffer;

  // Handling the initialization statement
  void handleForInit(Stmt* init, std::string &induc, std::string &valBegin) {
    if (init) {
      // Initialization as assignment expression
      if (auto assign = dyn_cast<BinaryOperator>(init)) {
        if (assign->isAssignmentOp()) {
          if (auto initVar = dyn_cast<VarDecl>(assign->getLHS()->getReferencedDeclOfCallee()))
            induc = initVar->getNameAsString();

          // Initialization with RHS as an integer
          if (auto initValInt = dyn_cast<IntegerLiteral>(assign->getRHS())) {
            valBegin = std::to_string((int)initValInt->getValue().roundToDouble());
          } else if (auto initDeclRef = dyn_cast<VarDecl>(assign->getRHS()->getReferencedDeclOfCallee())) {
            inputsBuffer.insert(initDeclRef);
            valBegin = initDeclRef->getNameAsString();
          }
        }
      }
      // Initialzation with a var declaration
      else if (auto varDeclStmt = dyn_cast<DeclStmt>(init)) {
        if (auto valDecl = dyn_cast<VarDecl>(varDeclStmt->getSingleDecl())) {
          induc = valDecl->getNameAsString();

          // Initialization with an integer
          if (auto varDeclInt = dyn_cast<IntegerLiteral>(valDecl->getInit()))
            valBegin = std::to_string((int)varDeclInt->getValue().roundToDouble());

          // Initialization with another variable
          else if (auto varDeclRef =
                       dyn_cast<VarDecl>(valDecl->getInit()->IgnoreImpCasts()->getReferencedDeclOfCallee())) {
            inputsBuffer.insert(varDeclRef);
            valBegin = varDeclRef->getNameAsString();
          }
        }
      }
    }
  }

  // Handling the condition expression
  void handleForCond(Expr* cond, std::string &valEnd) {
    if (cond) {
      if (auto bo = dyn_cast<BinaryOperator>(cond)) {
        auto boolRHS = bo->getRHS();
        auto boolLHS = bo->getLHS();
        if (auto condvarL = dyn_cast<VarDecl>(boolLHS->getReferencedDeclOfCallee())) {
          // For ForCondExpr like "i < n"
          if (auto condvarR = dyn_cast<VarDecl>(boolRHS->getReferencedDeclOfCallee())) {
            inputsBuffer.insert(condvarR);
            valEnd = condvarR->getNameAsString();
          }
          // For ForCondExpr like "i > 10"
          else if (auto condvalR = dyn_cast<IntegerLiteral>(boolRHS)) {
            valEnd = std::to_string((int)condvalR->getValue().roundToDouble());
          }
        }
      }
    }
  }

  // Handling the increment expression
  void handleForInc(Expr* inc, std::string &increment) {
    if (inc) {
      if (auto unaryOp = dyn_cast<UnaryOperator>(inc)) {
        if (unaryOp->isIncrementDecrementOp()) {
          if (unaryOp->isDecrementOp()) {
            increment = "-1";
          } else {
            increment = "1";
          }
        }
      }
    }
  }

  // Handling the body of the for loop
  void handleForBody(Stmt* body) {
    if (body) {
      if (auto bodyStmt = dyn_cast<CompoundStmt>(body)) {
        auto it = bodyStmt->children().begin();
        while (it != bodyStmt->children().end()) {
          // Checking for nested loops
          if (auto nestedFor = dyn_cast<ForStmt>(*it)) {
            VisitForStmt(nestedFor, true);
          }
          it++;
        }
      }
    }
  }
};

class FindForCondConsumer : public clang::ASTConsumer {
public:
  explicit FindForCondConsumer(ASTContext* Context) : Visitor(Context) {}

  virtual void HandleTranslationUnit(clang::ASTContext& Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  FindForCondVisitor Visitor;
};

class FindForCondAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& Compiler,
                                                                llvm::StringRef InFile) {
    return std::make_unique<FindForCondConsumer>(&Compiler.getASTContext());
  }
};

int main(int argc, const char** argv) {
  auto ExpectedParser = CommonOptionsParser::create(argc, argv, MatcherCategory);
  if (!ExpectedParser) {
    // Fail gracefully for unsupported options.
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  CommonOptionsParser& OptionsParser = ExpectedParser.get();

  ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

  return Tool.run(clang::tooling::newFrontendActionFactory<FindForCondAction>().get());
}