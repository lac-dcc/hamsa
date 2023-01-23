#ifndef FOR_INFO_TOOL
#define FOR_INFO_TOOL

#include "Kernel.hpp"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallSet.h"

/**
 * \class LoopInfoVisitor
 *
 * \brief Implementation of a recursive AST Visitor.
 *
 * This class implements a recursive AST Visitor, using Clang's RecursiveASTVisitor class.
 * The main goal is to extract information from loops, such as their induction variables,
 * bounds and inputs.
 */
class LoopInfoVisitor : public clang::RecursiveASTVisitor<LoopInfoVisitor> {
public:
  SeqKernel* root;

  /**
   * \brief Constructor method.
   * \param context ASTContext to be used by the visitor.
   */
  explicit LoopInfoVisitor(clang::ASTContext* context) : context(context) {
    this->root = new SeqKernel;
  }

  /**
   * \brief Destructor method.
   */
  ~LoopInfoVisitor() {
    delete this->root;

    for (auto it = this->loopKernels.begin(), end = this->loopKernels.end(); it != end; ++it)
      delete it->second;
  }

  /**
   * \brief Visit method to be applied to ForStmt nodes.
   * \param fstmt ForStmt node being currently visited.
   */
  bool VisitForStmt(clang::ForStmt* fstmt);

  /**
   * \brief Visit method to be applied to ForStmt nodes.
   * \param fstmt ForStmt node being currently visited.
   */
  bool VisitIfStmt(clang::IfStmt* ifstmt);

  /**
   * \brief Getter for the kernels attribute.
   * \return Hash table of kernels.
   */
  llvm::DenseMap<int64_t, LoopKernel*> getKernels();

private:
  clang::ASTContext* context; ///< ASTContext to be used by the visitor.
  llvm::DenseMap<int64_t, LoopKernel*> loopKernels; ///< Hash table of kernels identified during the Visitor's execution.
  llvm::DenseMap<int64_t, SeqKernel*> ifStmtParents;///< Hash table that associates a ifStmt id with its parent.
  llvm::SmallSet<clang::ValueDecl*, 8> inputsBuffer; ///< Container used to store the for's inputs.
  llvm::DenseMap<clang::ValueDecl*, std::string> bodyDeclarations; ///< Hash table of variables declared inside the loop's body.

  /**
   * \brief Depth-first traversal that searches for references to variables (inputs) and nested
   *        loops in a loop's body.
   * \param node Root of the subtree.
   * \param nested Flag that indicates if the current ForStmt is a nested for.
   * \param firstCall Flag to distinguish recursive calls from normal ones.
   */
  void traverseForBody(clang::Stmt* node, LoopKernel* kernel, bool firstCall = true);

    /**
   * \brief Depth-first traversal that searches for ForStmt in a IfStmt's body.
   * \param node Root of the subtree.
   * \param nested Flag that indicates if the current ForStmt is a nested for.
   */
  void traverseIfBody(clang::Stmt* node, int64_t& ifstmtId, CondKernel*& cond, bool isElse = false);

  /**
   * \brief Depth-first traversal that searches for references to variables in an expression.
   * \param node Root of the subtree.
   * \param kernel Kernel being visited.
   */
  void traverseExpr(clang::Stmt* node, LoopKernel* kernel);

  /**
   * \brief Auxiliary method used to handle the initialization statement of a for loop.
   * \param init Initialization statement.
   * \param kernel Kernel being visited.
   */
  void handleForInit(clang::Stmt* init, LoopKernel* kernel);

  /**
   * \brief Auxiliary method used to handle the condition expression.
   * \param cond Condition expression.
   * \param kernel Kernel being visited.
   */
  void handleForCond(clang::Expr* cond, LoopKernel* kernel);

  /**
   * \brief Auxiliary method used to handle the increment expression.
   * \param inc Increment expression.
   * \param kernel Kernel being visited.
   */
  void handleForInc(clang::Expr* inc, LoopKernel* kernel);

  /**
   * \brief Auxiliary method used to handle the body of the for loop.
   * \param body Statement that represents the for's body.
   * \param kernel Kernel being visited.
   */
  void handleForBody(clang::Stmt* body, LoopKernel* kernel);

  /**
   * \brief Auxiliary method used to handle the body of the If Stmt.
   * \param body Statement that represents the if's body.
   * \param kernel Kernel being visited.
   */
  void handleIfBody(clang::Stmt* body, CondKernel* kernel);
};

/**
 * \class LoopInfoConsumer
 *
 * \brief Class used to write generic actions on the AST.
 */
class LoopInfoConsumer : public clang::ASTConsumer {
public:
  explicit LoopInfoConsumer(clang::ASTContext* Context, std::string outputFile, std::string format)
      : visitor(Context), outputFile(outputFile), outputFormat(format) {}

  virtual void HandleTranslationUnit(clang::ASTContext& Context);

private:
  LoopInfoVisitor visitor;
  std::string outputFile;
  std::string outputFormat;
};

/**
 * \class LoopInfoAction
 *
 * \brief Class used to define a Clang plugin action.
 * To know more about plugins: https://youtu.be/SnP-8QM-TlI
 */
class LoopInfoAction : public clang::PluginASTAction {
protected:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& Compiler,
                                                                llvm::StringRef InFile) override {
    return std::make_unique<LoopInfoConsumer>(&Compiler.getASTContext(), outputFile, outputFormat);
  }

  bool ParseArgs(const clang::CompilerInstance& Compiler, const std::vector<std::string>& args) override;

private:
  std::string outputFormat = "txt";
  std::string outputFile = "output.txt";
};

static clang::FrontendPluginRegistry::Add<LoopInfoAction> X("hamsa", "get loop info");

#endif