#ifndef FOR_INFO_TOOL
#define FOR_INFO_TOOL
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
  bool VisitForStmt(ForStmt* fstmt, bool nested = false);

private:
  ASTContext* Context;

  // This container is used to store the for's inputs
  SmallSet<VarDecl*, 6> inputsBuffer;

  // Handling the initialization statement
  void handleForInit(Stmt* init, std::string& induc, std::string& valBegin);

  // Handling the condition expression
  void handleForCond(Expr* cond, std::string& valEnd);

  // Handling the increment expression
  void handleForInc(Expr* inc, std::string& increment);

  // Handling the body of the for loop
  void handleForBody(Stmt* body);
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


#endif