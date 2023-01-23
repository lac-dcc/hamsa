#include "Complexity.hpp"
#include "Printer.hpp"
#include "llvm/Support/raw_ostream.h"
#include <locale>

using namespace llvm;
using namespace clang;

std::string calculateSingleCost(LoopKernel* kernel, ASTContext& context) {
  std::string tempInit = Printer::getSourceCodeText(kernel->init, context);
  std::string tempLimit = Printer::getSourceCodeText(kernel->limit, context);
  std::string tempInc = Printer::getSourceCodeText(kernel->inc, context);
  std::string tempInduc = kernel->induc->getNameAsString();
  if (tempInc == tempInduc + "++")
    tempInc = "1";
  else if (tempInc == tempInduc + "--")
    tempInc = "-1";

  if (isdigit(tempInit[0]) && isdigit(tempLimit[0]) && isdigit(tempInc[0]))
    return "1";
  else if (isdigit(tempInit[0]) && !isdigit(tempLimit[0]) && (isdigit(tempInc[0]) || isdigit(tempInc[1])))
    return tempLimit;
  else if (!isdigit(tempInit[0]) && isdigit(tempLimit[0]) && (isdigit(tempInc[0]) || isdigit(tempInc[1])))
    return tempInit;
  else
    return "unknown";
}
