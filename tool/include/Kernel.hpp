#ifndef HAMSA_KERNEL
#define HAMSA_KERNEL

#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/SmallSet.h"
#include <string>

class KernelVisitor;

/**
 * \class Kernel
 *
 * \brief Data structure used to represent a kernel. It contains any information
 * that might be useful to infer the kernel's complexity.
 */
class Kernel {
public:
  Kernel* parent = nullptr;                    ///< Parent kernel (if there is any).
  llvm::SmallSet<clang::ValueDecl*, 8> inputs; ///< Set of inputs of the kernel.
  clang::SourceLocation begin;                 ///< Location of the beginning of the kernel.
  std::string complexity;                      ///< Kernel asymptotic complexity.
  int64_t id;                                  ///< Kernel id.

  virtual std::string accept(KernelVisitor* visitor) = 0;
  virtual ~Kernel() = default;
};

class SeqKernel : public Kernel {
public:
  llvm::SmallSet<Kernel*, 3> children;

  virtual std::string accept(KernelVisitor* visitor);
  // virtual std::string eval(clang::ASTContext& context);
};

class LoopKernel : public Kernel {
public:
  LoopKernel(int64_t id);
  ~LoopKernel();

  SeqKernel* child;      ///< Loop child.
  clang::VarDecl* induc; ///< Induction variable.
  clang::Expr* init;     ///< Induction variable's initial value.
  clang::Expr* limit;    ///< Induction variable's limit.
  clang::Expr* inc;      ///< Induction variable's increment at each iteration.

  virtual std::string accept(KernelVisitor* visitor);
};

class CondKernel : public Kernel {
public:
  CondKernel(int64_t id);
  ~CondKernel();

  SeqKernel* thenChild;
  SeqKernel* elseChild;

  virtual std::string accept(KernelVisitor* visitor);
};

#endif