#ifndef FOR_INFO_TOOL
#define FOR_INFO_TOOL
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/DenseMap.h"

using namespace clang;
using namespace llvm;

/**
 * \class LoopInfoVisitor
 *
 * \brief Implementation of a recursive AST Visitor.
 *
 * This class implements a recursive AST Visitor, using Clang's RecursiveASTVisitor class.
 * The main goal is to extract information from loops, such as their induction variables,
 * bounds and inputs.
 */
class LoopInfoVisitor : public RecursiveASTVisitor<LoopInfoVisitor> {
public:
  /**
   * \brief Constructor method.
   * \param context ASTContext to be used by the visitor.
   */
  explicit LoopInfoVisitor(ASTContext* context) : context(context) {}

  /**
   * \brief Visit method to be applied to ForStmt nodes.
   * \param fstmt ForStmt node being currently visited.
   * \param nested Flag that indicates if the current ForStmt is a nested for.
   */
  bool VisitForStmt(ForStmt* fstmt, bool nested = false);

private:
  ASTContext* context; ///< ASTContext to be used by the visitor.

  SmallSet<ValueDecl*, 8> inputsBuffer; ///< Container used to store the for's inputs.
  DenseMap<ValueDecl*, std::string> bodyDeclarations; ///< Hash table of variables declared inside the loop's body.

  /**
   * \brief Depth-first traversal that searches for references to variables (inputs) and nested
   *        loops in a loop's body.
   * \param node Root of the subtree.
   * \param nested Flag that indicates if the current ForStmt is a nested for.
   * \param firstCall Flag to distinguish recursive calls from normal ones.
   */
  void traverseForBody(Stmt* node, bool nested, bool firstCall = true);

  /**
   * \brief Depth-first traversal that searches for references to variables in an expression.
   * \param node Root of the subtree.
   */
  void traverseExpr(Stmt* node);

  /**
   * \brief Auxiliary method used to handle the initialization statement of a for loop.
   * \param init Initialization statement.
   * \param induc String that will hold the name of the induction variable's name.
   * \param valBegin String that will hold the initial value of the induction variable.
   */
  void handleForInit(Stmt* init, std::string& induc, std::string& valBegin);

  /**
   * \brief Auxiliary method used to handle the condition expression.
   * \param cond Condition expression.
   * \param valEnd String that will hold the final value of the induction variable.
   */
  void handleForCond(Expr* cond, std::string& valEnd);

  /**
   * \brief Auxiliary method used to handle the increment expression.
   * \param inc Increment expression.
   * \param increment String that will hold the value of the "step" in the increment expression.
   */
  void handleForInc(Expr* inc, std::string& increment);

  /**
   * \brief Auxiliary method used to handle the body of the for loop.
   * \param body Statement that represents the for's body.
   * \param nested Flag that indicates if the current ForStmt is a nested for.
   */
  void handleForBody(Stmt* body, bool nested);
};

/**
 * \class LoopInfoConsumer
 *
 * \brief Class used to write generic actions on the AST.
 */
class LoopInfoConsumer : public ASTConsumer {
public:
  explicit LoopInfoConsumer(ASTContext* Context) : visitor(Context) {}

  virtual void HandleTranslationUnit(ASTContext& Context) {
    visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  LoopInfoVisitor visitor;
};

/**
 * \class LoopInfoAction
 *
 * \brief Class used to define AST consumer-based frontend actions.
 */
class LoopInfoAction : public ASTFrontendAction {
public:
  virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance& Compiler, StringRef InFile) {
    return std::make_unique<LoopInfoConsumer>(&Compiler.getASTContext());
  }
};

#endif