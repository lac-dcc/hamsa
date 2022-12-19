#include "Complexity.hpp"
#include "Printer.hpp"
#include "llvm/Support/raw_ostream.h"
#include <locale>

using namespace llvm;
using namespace clang;

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
    return "unknown";
}

void inferComplexityAux(Kernel* kernel, ASTContext& Context) {
  if(!kernel->children.empty()) {
    std::string costAux = calculateSingleCost(kernel, Context) + "*(";
    for (Kernel* child : kernel->children) {
      inferComplexityAux(child, Context);
      costAux += child->complexity + "+";
    }
    costAux.pop_back();
    costAux += ")";
    kernel->complexity = costAux;
  } else {
    kernel->complexity = calculateSingleCost(kernel, Context);
  }
}

void inferComplexity(const DenseMap<int64_t, Kernel*>& kernels, ASTContext& Context) {
  for (auto& [id, kernel] : kernels) {
    if (kernel->parent == nullptr)
      inferComplexityAux(kernel, Context);
  }
}
