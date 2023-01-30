#include "Complexity.hpp"
#include "Printer.hpp"

using namespace llvm;
using namespace clang;

bool isNumber(Expr* expr) {
  if (auto num = dyn_cast<IntegerLiteral>(expr->IgnoreCasts()))
    return true;
  return false;
}

bool isSingleVar(Expr* expr) {
  if (auto ref = dyn_cast<DeclRefExpr>(expr->IgnoreCasts()))
    return true;
  return false;
}

std::string calculateSingleCost(LoopKernel* kernel, ASTContext& context) {
  std::string init = Printer::getSourceCodeText(kernel->init, context);
  std::string limit = Printer::getSourceCodeText(kernel->limit, context);
  std::string inc = Printer::getIncRepresentation(kernel->inc, context);

  bool initIsNumber = isNumber(kernel->init);
  bool limitIsNumber = isNumber(kernel->limit);

  if (initIsNumber && limitIsNumber) {
    // (int i = 0; i < 10; i++) || (int i = 10; i > 0; i--)
    if (((kernel->limitOp == "<" || kernel->limitOp == "<=") && inc[0] == '+') ||
        ((kernel->limitOp == ">" || kernel->limitOp == ">=") && inc[0] == '-'))
      return "1";
  } else if (initIsNumber && !limitIsNumber) {
    if (kernel->limitOp == "<" || kernel->limitOp == "<=") {
      // (int i = 0; i < n; i++)
      if (inc[0] == '+')
        return isSingleVar(kernel->limit) ? limit : "(" + limit + ")";
      // (int i = 0; i < n; i *= 2)
      if (inc[0] == '*')
        return "log(" + limit + ")";
    }
  } else if (!initIsNumber && limitIsNumber) {
    if (kernel->limitOp == ">" || kernel->limitOp == ">=") {
      // (int i = n; i > 0; i--)
      if (inc[0] == '-')
        return isSingleVar(kernel->limit) ? limit : "(" + limit + ")";
      // (int i = n; i > 0; i /= 2)
      if (inc[0] == '*')
        return "log(" + limit + ")";
    }
  } else if (!initIsNumber && !limitIsNumber) {
    if (kernel->limitOp == "<" || kernel->limitOp == "<=") {
      // (int i = n; i < m; i++)
      if (inc[0] == '+') {
        if (!isSingleVar(kernel->init))
          init = "(" + init + ")";
        if (!isSingleVar(kernel->limit))
          limit = "(" + limit + ")";
        return "((" + limit + "-" + init + ")" + "/" + inc.substr(1, inc.size()-1) + ")";
      }
    }
  }

  return "unknown construction: " + kernel->induc->getNameAsString() + ", <" + init + ", " + limit + ", " + inc + ">";
}