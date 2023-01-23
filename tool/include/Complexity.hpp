#ifndef HAMSA_COMPLEXITY
#define HAMSA_COMPLEXITY

#include "Kernel.hpp"
#include "llvm/ADT/DenseMap.h"
#include "clang/AST/ASTContext.h"

/**
 * \brief Calculate the cost of a single loop from its triplet information.
 * \param kernel Target kernel.
 * \param context ASTContext to be used by the function.
 */
std::string calculateSingleCost(LoopKernel* kernel, clang::ASTContext& context);

#endif