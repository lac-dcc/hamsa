#ifndef FOR_INFO_TOOL
#define FOR_INFO_TOOL
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallSet.h"

using namespace clang;
using namespace llvm;

/**
 * \struct Kernel
 *
 * \brief Data structure used to represent a kernel. It contains any information
 * that might be useful to infer the kernel's complexity.
 */
struct Kernel {
  VarDecl* induc;                 ///< Induction variable.
  Expr* init;                     ///< Induction variable's initial value.
  Expr* limit;                    ///< Induction variable's limit.
  Expr* inc;                      ///< Induction variable's increment at each iteration.
  SmallSet<ValueDecl*, 8> inputs; ///< Set of inputs of the kernel.
  Kernel* parent = nullptr;       ///< Parent kernel (if there is any).
};

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
   * \brief Destructor method.
   */
  ~LoopInfoVisitor() {
    for (auto it = kernels.begin(), end = kernels.end(); it != end; ++it)
      delete it->second;
  }

  /**
   * \brief Visit method to be applied to ForStmt nodes.
   * \param fstmt ForStmt node being currently visited.
   */
  bool VisitForStmt(ForStmt* fstmt);

  /**
   * \brief Getter for the kernels attribute.
   * \return Hash table of kernels.
   */
  DenseMap<int64_t, Kernel*> getKernels();

private:
  ASTContext* context; ///< ASTContext to be used by the visitor.

  DenseMap<int64_t, Kernel*> kernels;   ///< Hash table of kernels identified during the Visitor's execution.
  SmallSet<ValueDecl*, 8> inputsBuffer; ///< Container used to store the for's inputs.
  DenseMap<ValueDecl*, std::string> bodyDeclarations; ///< Hash table of variables declared inside the loop's body.

  /**
   * \brief Depth-first traversal that searches for references to variables (inputs) and nested
   *        loops in a loop's body.
   * \param node Root of the subtree.
   * \param nested Flag that indicates if the current ForStmt is a nested for.
   * \param firstCall Flag to distinguish recursive calls from normal ones.
   */
  void traverseForBody(Stmt* node, Kernel* kernel, bool firstCall = true);

  /**
   * \brief Depth-first traversal that searches for references to variables in an expression.
   * \param node Root of the subtree.
   * \param kernel Kernel being visited.
   */
  void traverseExpr(Stmt* node, Kernel* kernel);

  /**
   * \brief Auxiliary method used to handle the initialization statement of a for loop.
   * \param init Initialization statement.
   * \param kernel Kernel being visited.
   */
  void handleForInit(Stmt* init, Kernel* kernel);

  /**
   * \brief Auxiliary method used to handle the condition expression.
   * \param cond Condition expression.
   * \param kernel Kernel being visited.
   */
  void handleForCond(Expr* cond, Kernel* kernel);

  /**
   * \brief Auxiliary method used to handle the increment expression.
   * \param inc Increment expression.
   * \param kernel Kernel being visited.
   */
  void handleForInc(Expr* inc, Kernel* kernel);

  /**
   * \brief Auxiliary method used to handle the body of the for loop.
   * \param body Statement that represents the for's body.
   * \param kernel Kernel being visited.
   */
  void handleForBody(Stmt* body, Kernel* kernel);
};

/**
 * \class LoopInfoConsumer
 *
 * \brief Class used to write generic actions on the AST.
 */
class LoopInfoConsumer : public ASTConsumer {
public:
  explicit LoopInfoConsumer(ASTContext* Context) : visitor(Context) {}

  virtual void HandleTranslationUnit(ASTContext& Context);

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