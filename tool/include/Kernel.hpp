#ifndef HAMSA_KERNEL
#define HAMSA_KERNEL

#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/SmallSet.h"
#include <string>

/**
 * \class Kernel
 *
 * \brief Data structure used to represent a kernel. It contains any information
 * that might be useful to infer the kernel's complexity.
 */
class Kernel {
public:
  int64_t id;                                  ///< Kernel id.
  llvm::SmallSet<clang::ValueDecl*, 8> inputs; ///< Set of inputs of the kernel.
  clang::SourceLocation begin;                 ///< Location of the beginning of the kernel.
  std::string complexity;                      ///< Kernel asymptotic complexity.

  virtual std::string eval(clang::ASTContext& context) = 0; ///< Kernel asymptotic complexity.
  virtual ~Kernel() = default;
};

class SeqKernel : public Kernel {
public:
  llvm::SmallSet<Kernel*, 3> children;
  Kernel* parent = nullptr;                    ///< Parent kernel (if there is any).
  virtual std::string eval(clang::ASTContext& context);
};

class LoopKernel : public Kernel {
public:
  SeqKernel child;              ///< Loop child.
  clang::VarDecl* induc;        ///< Induction variable.
  clang::Expr* init;            ///< Induction variable's initial value.
  clang::Expr* limit;           ///< Induction variable's limit.
  clang::Expr* inc;             ///< Induction variable's increment at each iteration.
  SeqKernel* parent = nullptr;     ///< Parent kernel (if there is any).
  virtual std::string eval(clang::ASTContext& context);
};

class CondKernel : public Kernel {
public:
  SeqKernel leftChild;
  SeqKernel rightChild;
  SeqKernel* parent = nullptr;                              ///< Parent kernel (if there is any).
  virtual std::string eval(clang::ASTContext& context);
};

#endif