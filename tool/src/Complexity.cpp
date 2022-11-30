#include "Complexity.hpp"
#include "llvm/Support/raw_ostream.h"
#include "Printer.hpp"
#include <locale>

using namespace llvm;
using namespace clang;

/*
 Cost = (|limit - init + 1|) / | inc |
*/

std::string calculateSingleCost(Kernel* kernel, ASTContext& Context) {
  std::string tempInit = Printer::getSourceCodeText(kernel->init, Context);
  std::string tempLimit = Printer::getSourceCodeText(kernel->limit, Context);
  std::string tempInc = Printer::getSourceCodeText(kernel->inc, Context);
  std::string tempInduc = kernel->induc->getNameAsString();
  if(tempInc == tempInduc+"++") tempInc = "1";
  else if(tempInc == tempInduc+"--") tempInc = "-1";

  if (isdigit(tempInit[0]) && isdigit(tempLimit[0]) && isdigit(tempInc[0]))
    return "1";
  else if (isdigit(tempInit[0]) && !isdigit(tempLimit[0]) && (isdigit(tempInc[0]) || isdigit(tempInc[1])))
    return tempLimit;
  else if (!isdigit(tempInit[0]) && isdigit(tempLimit[0]) && (isdigit(tempInc[0]) || isdigit(tempInc[1])))
    return tempInit;
  else
    return "x";
}

void inferComplexity(const DenseMap<int64_t, Kernel*>& kernels, ASTContext& Context) {
    for(auto& [id, kernel] : kernels) {
      auto tempCost = calculateSingleCost(kernel, Context);
      auto auxKernel = kernel->parent;

      while(auxKernel != nullptr) {
        tempCost = tempCost + "*" + calculateSingleCost(auxKernel, Context);
        auxKernel = auxKernel->parent;
      }

      // outs() << id << ": " << kernel->induc->getNameAsString() << " - Cost: " << tempCost <<"\n";
      kernel->complexity = tempCost;
    }
}
