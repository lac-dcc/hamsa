#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;
using namespace clang::tooling;
using namespace llvm;

static llvm::cl::OptionCategory MatcherCategory("matcher options");

class FindForCondVisitor : public RecursiveASTVisitor<FindForCondVisitor> {
public:
  explicit FindForCondVisitor(ASTContext *Context) : Context(Context) {}

  bool VisitForStmt(ForStmt *fstmt) {
    handleForInit(fstmt->getInit());
    handleForCond(fstmt->getCond());
    handleForInc(fstmt->getInc());
    handleForBody(fstmt->getBody());
    return true;
  }

private:
  ASTContext *Context;

  // Handling the initialization statement
  void handleForInit(Stmt *init) {
    if (init) {
      // Initialization as assignment expression
      if (auto assign = dyn_cast<BinaryOperator>(init)) {
        if (assign->isAssignmentOp()) {
          if (auto initVar = dyn_cast<VarDecl>(
                  assign->getLHS()->getReferencedDeclOfCallee()))
            outs() << initVar->getNameAsString() << ", <";

          // Initialization with RHS as an integer
          if (auto initValInt = dyn_cast<IntegerLiteral>(assign->getRHS())) {
            outs() << (int)initValInt->getValue().roundToDouble() << ", ";
          }
          // Initialization with RHS as another variable
          else if (auto initDeclRef = dyn_cast<VarDecl>(
                       assign->getRHS()->getReferencedDeclOfCallee())) {
            outs() << initDeclRef->getNameAsString() << ", ";
          }
        }
      }
      // Initialzation with a var declaration
      else if (auto varDeclStmt = dyn_cast<DeclStmt>(init)) {
        if (auto valDecl = dyn_cast<VarDecl>(varDeclStmt->getSingleDecl())) {
          outs() << valDecl->getNameAsString() << ", <";
          // Initialization with an integer
          if (auto varDeclInt = dyn_cast<IntegerLiteral>(valDecl->getInit())) {
            outs() << varDeclInt->getValue() << ", ";
          }
          // Initialization with another variable
          else if (auto varDeclRef =
                       dyn_cast<VarDecl>(valDecl->getInit()
                                             ->IgnoreImpCasts()
                                             ->getReferencedDeclOfCallee())) {
            outs() << varDeclRef->getNameAsString() << ", ";
          }
        }
      }
    }
  }

  // Handling the condition expression
  void handleForCond(Expr *cond) {
    if (cond) {
      if (auto bo = dyn_cast<BinaryOperator>(cond)) {
        auto boolRHS = bo->getRHS();
        auto boolLHS = bo->getLHS();
        if (auto condvarL =
                dyn_cast<VarDecl>(boolLHS->getReferencedDeclOfCallee())) {
          // For ForCondExpr like "i < n"
          if (auto condvarR =
                  dyn_cast<VarDecl>(boolRHS->getReferencedDeclOfCallee())) {
            outs() << condvarR->getNameAsString() << ", ";
          }
          // For ForCondExpr like "i > 10"
          else if (auto condvalR = dyn_cast<IntegerLiteral>(boolRHS)) {
            outs() << (int)condvalR->getValue().roundToDouble() << ", ";
          }
        }
      }
    }
  }

  // Handling the increment expression
  void handleForInc(Expr *inc) {
    if (inc) {
      if (auto unaryOp = dyn_cast<UnaryOperator>(inc)) {
        if (unaryOp->isIncrementDecrementOp()) {
          if (unaryOp->isDecrementOp()) {
            outs() << "-1> ";
          } else {
            outs() << "1> ";
          }
        }
      }
    }
  }

  // Handling the body of the for loop
  void handleForBody(Stmt *body) {
    if (body) {
      if (auto bodyStmt = dyn_cast<CompoundStmt>(body)) {
        auto it = bodyStmt->children().begin();
        outs() << "{\n";
        while (it != bodyStmt->children().end()) {
          // Checking for nested loops
          if (auto nestedFor = dyn_cast<ForStmt>(*it)) {
            VisitForStmt(nestedFor);
          }
          it++;
        }
        outs() << "}\n";
      }
    }
  }
};

class FindForCondConsumer : public clang::ASTConsumer {
public:
  explicit FindForCondConsumer(ASTContext *Context) : Visitor(Context) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  FindForCondVisitor Visitor;
};

class FindForCondAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::make_unique<FindForCondConsumer>(&Compiler.getASTContext());
  }
};

int main(int argc, const char **argv) {
  auto ExpectedParser =
      CommonOptionsParser::create(argc, argv, MatcherCategory);
  if (!ExpectedParser) {
    // Fail gracefully for unsupported options.
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  CommonOptionsParser &OptionsParser = ExpectedParser.get();

  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  return Tool.run(
      clang::tooling::newFrontendActionFactory<FindForCondAction>().get());
}