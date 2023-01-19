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

std::string DotKernelVisitor::visit(LoopKernel* kernel) {
  std::string link = "";
  if(kernel->child->children.size() > 0)
    link+=std::to_string(kernel->id)+" -> "+std::to_string(kernel->child->id)+"\n"+this->visit(kernel->child);
  return link;
}

std::string DotKernelVisitor::visit(SeqKernel* kernel) {
 std::string links = "";
 std::string identifier = std::to_string(kernel->id);
 std::string label = identifier + "[label=\"Seq\"]\n";
 for(auto child : kernel->children) {
    links += identifier + " -> " + std::to_string(child->id) + "\n" + child->accept(this);
 }

 return label + links;
}

std::string DotKernelVisitor::visit(CondKernel* kernel) {
 std::string links = "";
 std::string label = std::to_string(kernel->id) + "[label=\"Cond\"]\n";
 links += std::to_string(kernel->id) + " -> " + std::to_string(kernel->thenChild->id) + "\n" + this->visit(kernel->thenChild);
 if(kernel->elseChild->children.size() > 0)
  links += std::to_string(kernel->id) + " -> " + std::to_string(kernel->elseChild->id) + "\n" + this->visit(kernel->elseChild);
 return label + links;
}