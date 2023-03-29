#ifndef TREE_BUILDER
#define TREE_BUILDER

#include "Kernel.hpp"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallSet.h"
#include <string>
#include <map>

struct TensilicaVar {
  std::string origin;
  int dimIndex = -1;
};

struct TensilicaTree {
  ~TensilicaTree() { delete this->root; }
  std::map<std::string, TensilicaVar>
      tensilicaVariables; ///< Hash table that maps variable names to tensilica function names.
  SeqKernel* root;        ///< Kernels tree root.
};

/**
 * \class TreeBuilderVisitor
 *
 * \brief Implementation of a recursive AST Visitor.
 *
 * This class implements a recursive AST Visitor, using Clang's RecursiveASTVisitor class.
 * The main goal is to extract information from loops, such as their induction variables,
 * bounds and inputs.
 */
class TreeBuilderVisitor : public clang::RecursiveASTVisitor<TreeBuilderVisitor> {
public:
  std::map<std::string, TensilicaVar>
      tensilicaVariables; ///< Hash table that maps variable names to tensilica function names.
  SeqKernel* root;        ///< Kernels tree root.
  llvm::DenseMap<clang::FunctionDecl*, TensilicaTree*>* kernelFunctions;

  /**
   * \brief Constructor method.
   * \param context ASTContext to be used by the visitor.
   */
  explicit TreeBuilderVisitor(clang::ASTContext* context) : context(context) { this->root = new SeqKernel; }

  /**
   * \brief Destructor method.
   */
  ~TreeBuilderVisitor() { delete this->root; }

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

  void clearState();

private:
  clang::ASTContext* context; ///< ASTContext to be used by the visitor.
  llvm::DenseMap<int64_t, LoopKernel*>
      loopKernels; ///< Hash table of kernels identified during the Visitor's execution.
  llvm::DenseMap<int64_t, SeqKernel*> ifStmtParents; ///< Hash table that associates a ifStmt id with its parent.
  llvm::DenseMap<clang::ValueDecl*, LoopKernel*> forVariables; ///< Container used to store for's triplet variables.
  llvm::DenseMap<clang::ValueDecl*, std::string>
      bodyDeclarations; ///< Hash table of variables declared inside the loop's body.

  std::string currCondLowerLim = "";
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
  void traverseIfBody(clang::Stmt* node, CondKernel*& cond, bool isElse = false);

  bool hasForVariable(clang::Stmt* node, std::string& tensilicaCondRHS, bool& hasPitch);

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

  void getVWDecl(clang::Stmt* initialVal);
};

class KernelFunctionVisitor : public clang::RecursiveASTVisitor<KernelFunctionVisitor> {
public:
  /**
   * \brief Constructor method.
   * \param context ASTContext to be used by the visitor.
   */
  explicit KernelFunctionVisitor(clang::ASTContext* context, TreeBuilderVisitor* visitor)
      : context(context), loopVisitor(visitor) {
    this->loopVisitor->kernelFunctions = &this->kernelFunctions;
  }

  ~KernelFunctionVisitor() {
    for (auto& [_, kernelTree] : kernelFunctions) {
      delete kernelTree;
    }
  }

  bool VisitFunctionDecl(clang::FunctionDecl* funcDecl);

  llvm::DenseMap<clang::FunctionDecl*, TensilicaTree*> kernelFunctions;

private:
  clang::ASTContext* context; ///< ASTContext to be used by the visitor.
  TreeBuilderVisitor* loopVisitor;
};

/**
 * \class TreeBuilderConsumer
 *
 * \brief Class used to write generic actions on the AST.
 */
class TreeBuilderConsumer : public clang::ASTConsumer {
public:
  explicit TreeBuilderConsumer(clang::ASTContext* Context, std::string outputFile, std::string format)
      : visitor(Context), outputFile(outputFile), outputFormat(format) {}

  virtual void HandleTranslationUnit(clang::ASTContext& Context);

private:
  TreeBuilderVisitor visitor;
  std::string outputFile;
  std::string outputFormat;
};

/**
 * \class TreeBuilderAction
 *
 * \brief Class used to define a Clang plugin action.
 * To know more about Clang plugins: https://youtu.be/SnP-8QM-TlI
 */
class TreeBuilderAction : public clang::PluginASTAction {
protected:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& Compiler,
                                                                llvm::StringRef InFile) override {
    return std::make_unique<TreeBuilderConsumer>(&Compiler.getASTContext(), outputFile, outputFormat);
  }

  bool ParseArgs(const clang::CompilerInstance& Compiler, const std::vector<std::string>& args) override;

private:
  std::string outputFormat = "txt";
  std::string outputFile = "output.txt";
};

static clang::FrontendPluginRegistry::Add<TreeBuilderAction> X("hamsa", "get loop info");

#endif