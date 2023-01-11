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
private:
  int64_t id;                                  ///< Kernel id.
  llvm::SmallSet<clang::ValueDecl*, 8> inputs; ///< Set of inputs of the kernel.
  clang::SourceLocation begin;                 ///< Location of the beginning of the kernel.

public:
  virtual std::string eval() = 0; ///< Kernel asymptotic complexity.
};

class LoopKernel : public Kernel {
public:
  Kernel* child;         ///< Loop child.
  clang::VarDecl* induc; ///< Induction variable.
  clang::Expr* init;     ///< Induction variable's initial value.
  clang::Expr* limit;    ///< Induction variable's limit.
  clang::Expr* inc;      ///< Induction variable's increment at each iteration.
  virtual std::string eval();
};

class SeqKernel : public Kernel {
public:
  Kernel* leftChild;
  Kernel* rightChild;
  virtual std::string eval();
};

class CondKernel : public Kernel {
public:
  Kernel* leftChild;
  Kernel* rightChild;
  virtual std::string eval();
};

#endif