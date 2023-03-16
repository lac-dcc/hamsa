#ifndef HAMSA_KERNEL
#define HAMSA_KERNEL

#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/SmallSet.h"
#include <string>

template <class T> class KernelVisitor;

/**
 * \class Kernel
 *
 * \brief Abstract class for a Kernel. It doesn't implement any methods, but contains
 * attributes that are used by other types of Kernel.
 */
class Kernel {
public:
  llvm::SmallSet<clang::ValueDecl*, 8> inputs; ///< Set of inputs of the kernel.
  clang::SourceLocation begin;                 ///< Location of the beginning of the kernel.
  std::string complexity;                      ///< Kernel asymptotic complexity.
  int64_t id;                                  ///< Kernel id.

  /**
   * \brief Method that accepts the visit of a KernelVisitor. This is used for double dispatch.
   * \param visitor KernelVisitor instance that will apply the visit.
   */
  virtual std::string accept(KernelVisitor<std::string>* visitor) = 0;
  virtual ~Kernel() = default;
};

/**
 * \class SeqKernel
 *
 * \brief Class that represents a sequence of one or more kernels. Every kernel's
 * children is of this type.
 */
class SeqKernel : public Kernel {
public:
  ~SeqKernel();

  llvm::SmallSet<Kernel*, 3> children; ///< Set of child kernels
  Kernel* parent = nullptr;            ///< Parent kernel (if there is any).

  virtual std::string accept(KernelVisitor<std::string>* visitor);
};

/**
 * \class LoopKernel
 *
 * \brief Class that represents a single for-loop kernel.
 */
class LoopKernel : public Kernel {
public:
  LoopKernel(int64_t id);
  ~LoopKernel();

  SeqKernel* parent = nullptr; ///< Parent kernel (if there is any).
  SeqKernel* child;            ///< Child kernel.
  clang::VarDecl* induc;       ///< Induction variable.
  clang::Expr* init;           ///< Induction variable's initial value.
  clang::Expr* limit;          ///< Induction variable's limit.
  clang::Expr* inc;            ///< Induction variable's increment at each iteration.
  std::string limitOp;         ///< Limit operator.

  virtual std::string accept(KernelVisitor<std::string>* visitor);
};

/**
 * \class CondKernel
 *
 * \brief Class that represents a Kernel that is bound to an if statement.
 */
class CondKernel : public Kernel {
public:
  CondKernel(int64_t id, bool hasElse);
  ~CondKernel();

  clang::Expr* condition;      ///< Condition of the if statement.
  SeqKernel* parent = nullptr; ///< Parent kernel (if there is any).
  SeqKernel* thenChild;        ///< Child kernel for the "then" branch.
  SeqKernel* elseChild;        ///< Child kernel for the "else" branch.

  virtual std::string accept(KernelVisitor<std::string>* visitor);
};

#endif