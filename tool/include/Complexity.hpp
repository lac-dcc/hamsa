#ifndef HAMSA_COMPLEXITY
#define HAMSA_COMPLEXITY
#include "Kernel.hpp"
#include "llvm/ADT/DenseMap.h"

/**
 * \brief Infer the complexity of each Kernel in a map of Kernels.
 * \param kernels Hash map of Kernels.
 */
void inferComplexity(const llvm::DenseMap<int64_t, Kernel*>& kernels);

#endif