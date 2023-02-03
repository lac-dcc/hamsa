#include "Complexity.hpp"
#include "Printer.hpp"

using namespace llvm;
using namespace clang;

bool isNumber(Expr* expr) {
  if (auto num = dyn_cast<IntegerLiteral>(expr->IgnoreCasts())) {
    return true;
  }
  return false;
}

std::string calculateSingleCost(LoopKernel* kernel, ASTContext& context) {
  // (limit + inc - init - 1) / inc 
  // ou (|limit + inc - init| - 1) / |inc|

  std::string init = Printer::getSourceCodeText(kernel->init, context);
  std::string limit = Printer::getSourceCodeText(kernel->limit, context);
  std::string inc = Printer::getIncRepresentation(kernel->inc, context);
  char incSignal = inc[0];
  inc = inc.substr(1, inc.size() - 1);
  bool initIsNumber = isNumber(kernel->init);
  bool limitIsNumber = isNumber(kernel->limit);
  std::string iterations = "(";

  if (!limitIsNumber || (limitIsNumber && limit != "0"))
    iterations += limit + " ";
  iterations += incSignal;
  iterations += " " + inc;
  if (!initIsNumber || (initIsNumber && init != "0"))
    iterations += " - " + init;

  if(kernel->limitOp == "<")
    iterations += " - 1";
  if(kernel->limitOp == ">")
    iterations += " + 1";
    
  if (incSignal == '-')
    iterations += ") // (-" + inc + ")";
  else
    iterations += ") // " + inc;
  return " (" + iterations + ")";
}
