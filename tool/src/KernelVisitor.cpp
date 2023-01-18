#include "KernelVisitor.hpp"
#include "Printer.hpp"

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

std::string ComplexityKernelVisitor::visit(LoopKernel* kernel) {
  kernel->complexity = calculateSingleCost(kernel, *this->context) + "*" + this->visit(kernel->child);
  if (kernel->complexity[kernel->complexity.size() - 1] == '*')
    kernel->complexity.pop_back();
  return kernel->complexity;
}

std::string ComplexityKernelVisitor::visit(SeqKernel* kernel) {
  kernel->complexity = "";
  for (Kernel* child : kernel->children) {
    kernel->complexity += child->accept(this) + "+";
  }

  if (kernel->complexity.size() > 0)
    kernel->complexity.pop_back();

  if (kernel->children.size() >= 2)
    kernel->complexity = "(" + kernel->complexity + ")";

  return kernel->complexity;
}

std::string ComplexityKernelVisitor::visit(CondKernel* kernel) {
  kernel->complexity = this->visit(kernel->thenChild);

  if (kernel->elseChild != nullptr) {
    kernel->complexity = "(" + kernel->complexity + " | " + this->visit(kernel->elseChild) + ")";
  } 

  return kernel->complexity;
}