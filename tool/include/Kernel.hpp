#ifndef HAMSA_KERNEL
#define HAMSA_KERNEL

#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/SmallSet.h"
#include <string>

/**
 * \struct Kernel
 *
 * \brief Data structure used to represent a kernel. It contains any information
 * that might be useful to infer the kernel's complexity.
 */
struct Kernel {
  clang::VarDecl* induc;                       ///< Induction variable.
  clang::Expr* init;                           ///< Induction variable's initial value.
  clang::Expr* limit;                          ///< Induction variable's limit.
  clang::Expr* inc;                            ///< Induction variable's increment at each iteration.
  llvm::SmallSet<clang::ValueDecl*, 8> inputs; ///< Set of inputs of the kernel.
  clang::SourceLocation begin;                 ///< Location of the beginning of the kernel.
  std::string complexity = "";                 ///< Kernel asymptotic complexity.
  Kernel* parent = nullptr;                    ///< Parent kernel (if there is any).
};

#endif