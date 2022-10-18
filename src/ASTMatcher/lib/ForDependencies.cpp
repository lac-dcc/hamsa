// https://clang.llvm.org/docs/LibASTMatchersReference.html

// Declares clang::SyntaxOnlyAction.
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
// Declares llvm::cl::extrahelp.
#include "llvm/Support/CommandLine.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace llvm;
using namespace clang::ast_matchers;
using namespace clang::tooling;


static cl::OptionCategory MatcherCategory("matcher options");

StatementMatcher forInitDecl = forStmt(
                            hasLoopInit(
                              declStmt(
                                hasSingleDecl(
                                  varDecl(
                                    hasInitializer(
                                      integerLiteral().bind("initVal"))).bind("initVar")))));

StatementMatcher forInitAssign = forStmt(
                            hasLoopInit(
                              binaryOperator(
                                isAssignmentOperator(),
                                hasLHS(expr().bind("initVar")),
                                hasRHS(expr().bind("initVal")))));

StatementMatcher forCond =
  forStmt(
    hasCondition(
      binaryOperator(
        isComparisonOperator(),
        hasLHS(ignoringParenImpCasts(
          declRefExpr(
            to(
              varDecl(
                hasType(isInteger())))))),
        hasRHS(
          expr(
            hasType(
              isInteger())))).bind("forCondition")));

StatementMatcher forInc = forStmt(
                            hasIncrement(
                              stmt().bind("loopInc")));

StatementMatcher forBodyInput = 
  forStmt(
    hasBody(
      compoundStmt(
        hasAnySubstatement(
          expr(
            findAll(
              declRefExpr().bind("bodyInput")))))));

class LoopPrinter: public MatchFinder::MatchCallback {

public:
  virtual void run(const MatchFinder::MatchResult &Result) {
    if (const clang::Expr *InitVar = Result.Nodes.getNodeAs<clang::Expr>("initVar")) {
      outs() << "---Init var---\n";
    	InitVar->dump();
    }

    if (const clang::Decl *InitVar = Result.Nodes.getNodeAs<clang::Decl>("initVar")) {
      outs() << "---Init var---\n";
    	InitVar->dump();
    }

    if (const clang::Expr *InitVal = Result.Nodes.getNodeAs<clang::Expr>("initVal")) {
      outs() << "---Init value---\n";
    	InitVal->dump();
    }

    if (const clang::BinaryOperator *ForCond = Result.Nodes.getNodeAs<clang::BinaryOperator>("forCondition")) {
      outs() << "---For condition---\n";
    	ForCond->dump();
      outs() << "---Condition LHS---\n";
      ForCond->getLHS()->dump();
      outs() << "---Condition RHS---\n";
      ForCond->getRHS()->dump();
    }

    if (const clang::Stmt *LoopInc = Result.Nodes.getNodeAs<clang::Stmt>("loopInc")) {
      outs() << "---Increment---\n";
    	LoopInc->dump();
    }
    
    if (const clang::DeclRefExpr *BodyInput = Result.Nodes.getNodeAs<clang::DeclRefExpr>("bodyInput")) {
      outs() << "---Body input---\n";
    	BodyInput->dump();
    }
  }
};


int main(int argc, const char **argv) {
  auto ExpectedParser = CommonOptionsParser::create(argc, argv, MatcherCategory);

  if (!ExpectedParser) {
    // Fail gracefully for unsupported options.
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }

  CommonOptionsParser& OptionsParser = ExpectedParser.get();

  ClangTool Tool(OptionsParser.getCompilations(), 
                 OptionsParser.getSourcePathList());

  LoopPrinter Printer;
	MatchFinder Finder;
	Finder.addMatcher(traverse(clang::TK_IgnoreUnlessSpelledInSource, forInitDecl), &Printer);
	Finder.addMatcher(traverse(clang::TK_IgnoreUnlessSpelledInSource, forInitAssign), &Printer);
	Finder.addMatcher(traverse(clang::TK_IgnoreUnlessSpelledInSource, forCond), &Printer);
	Finder.addMatcher(traverse(clang::TK_IgnoreUnlessSpelledInSource, forInc), &Printer);
	Finder.addMatcher(traverse(clang::TK_IgnoreUnlessSpelledInSource, forBodyInput), &Printer);

  return Tool.run(newFrontendActionFactory(&Finder).get());
}


