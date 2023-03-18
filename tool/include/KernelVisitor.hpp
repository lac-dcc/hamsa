#ifndef KERNEL_VISITOR
#define KERNEL_VISITOR

#include "clang/AST/ASTContext.h"
#include <unordered_map>
#include <stack>
#include <string>

class Kernel;
class LoopKernel;
class SeqKernel;
class CondKernel;
class CallKernel;

/**
 * \class KernelVisitor
 *
 * \brief Implementation of a Visitor that traverses the Kernels tree.
 *
 * This class implements a recursive Kernel Visitor.
 * The main goal is traverse the kernels tree to do an action.
 */
template <class T> class KernelVisitor {
public:
  clang::ASTContext* context; ///< Clang AST context.

  /**
   * \brief Visit method to be applied to LoopKernel nodes.
   * \param kernel LoopKernel node being currently visited.
   */
  virtual T visit(LoopKernel* kernel) = 0;

  /**
   * \brief Visit method to be applied to SeqKernel nodes.
   * \param kernel ForStmt node being currently visited.
   */
  virtual T visit(SeqKernel* kernel) = 0;

  /**
   * \brief Visit method to be applied to CondKernel nodes.
   * \param kernel ForStmt node being currently visited.
   */
  virtual T visit(CondKernel* kernel) = 0;

  virtual T visit(CallKernel* kernel) = 0;

protected:
  /**
   * \brief KernelVisitor constructor.
   * \param context Clang AST context.
   */
  KernelVisitor(clang::ASTContext* context) : context(context) {}
  KernelVisitor() {}
};

/**
 * \class ComplexityKernelVisitor
 *
 * \brief Implementation of a Visitor that traverses the Kernels tree.
 *
 * This class implements a recursive Kernel Visitor.
 * The main goal is traverse the kernels tree to do extract the kernels complexity.
 */
class ComplexityKernelVisitor : public KernelVisitor<std::string> {
public:
  /**
   * \brief ComplexityKernelVisitor constructor.
   * \param context Clang AST context.
   */
  ComplexityKernelVisitor(clang::ASTContext* context) : KernelVisitor<std::string>(context) {}

  virtual std::string visit(LoopKernel* kernel);
  virtual std::string visit(SeqKernel* kernel);
  virtual std::string visit(CondKernel* kernel);
  virtual std::string visit(CallKernel* kernel) { return ""; }
};

/**
 * \class TxtKernelVisitor
 *
 * \brief Implementation of a Visitor that traverses the Kernels tree.
 *
 * This class implements a recursive Kernel Visitor.
 * The main goal is traverse the kernels tree to generate the .txt output.
 */
class TxtKernelVisitor : public KernelVisitor<std::string> {
public:
  /**
   * \brief TxtKernelVisitor constructor.
   * \param context Clang AST context.
   */
  TxtKernelVisitor(clang::ASTContext* context) : KernelVisitor<std::string>(context) {}

  virtual std::string visit(LoopKernel* kernel);
  virtual std::string visit(SeqKernel* kernel);
  virtual std::string visit(CondKernel* kernel);
  virtual std::string visit(CallKernel* kernel) { return ""; }
};

/**
 * \class DotKernelVisitor
 *
 * \brief Implementation of a Visitor that traverses the Kernels tree.
 *
 * This class implements a recursive Kernel Visitor.
 * The main goal is traverse the kernels tree to generate the .dot output.
 */
class DotKernelVisitor : public KernelVisitor<std::string> {
public:
  /**
   * \brief DotKernelVisitor constructor.
   * \param context Clang AST context.
   */
  DotKernelVisitor(clang::ASTContext* context) : KernelVisitor<std::string>(context) {}

  virtual std::string visit(LoopKernel* kernel);
  virtual std::string visit(SeqKernel* kernel);
  virtual std::string visit(CondKernel* kernel);
  virtual std::string visit(CallKernel* kernel) { return ""; }
};

/**
 * \class TensilicaKernelVisitor
 *
 * \brief Implementation of a Visitor that traverses the Kernels tree.
 *
 * This class implements a recursive Kernel Visitor.
 * The main goal is traverse the kernels tree to generate the perfModel output.
 */
class TensilicaKernelVisitor : public KernelVisitor<std::string> {
private:
  enum TPTypes { TPLoops, TPCond, TPSeq };
  std::stack<TPTypes> TPContext;
public:
  std::unordered_map<std::string, std::string> visitedFunctions;
  TensilicaKernelVisitor() {}
  /**
   * \brief TensilicaKernelVisitor constructor.
   * \param context Clang AST context.
   */
  TensilicaKernelVisitor(clang::ASTContext* context) : KernelVisitor<std::string>(context) {}

  virtual std::string visit(LoopKernel* kernel);
  virtual std::string visit(SeqKernel* kernel);
  virtual std::string visit(CondKernel* kernel);
  virtual std::string visit(CallKernel* kernel);
};

#endif