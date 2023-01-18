#include "Kernel.hpp"
#include "Printer.hpp"
#include <locale>

using namespace clang;


LoopKernel::LoopKernel(int64_t id) : id(id) {
  this->child = new SeqKernel;
}

LoopKernel::~LoopKernel() {
  delete this->child;
}

CondKernel::CondKernel() {
  this->thenChild = new SeqKernel;
}

CondKernel::~CondKernel() {
  delete this->thenChild;
  if(this->elseChild != nullptr)
    delete this->elseChild;
}

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

std::string LoopKernel::eval(ASTContext& context) {
  this->complexity = calculateSingleCost(this, context) + "*" + this->child->eval(context);
  if (this->complexity[this->complexity.size() - 1] == '*')
    this->complexity.pop_back();
  return this->complexity;
}

std::string SeqKernel::eval(ASTContext& context) {
  this->complexity = "";
  for (Kernel* child : this->children) {
    this->complexity += child->eval(context) + "+";
  }

  if (this->complexity.size() > 0)
    this->complexity.pop_back();

  if (this->children.size() >= 2)
    this->complexity = "(" + this->complexity + ")";

  return this->complexity;
}

std::string CondKernel::eval(ASTContext& context) {
  this->complexity = "(" + thenChild->eval(context) + " | " + elseChild->eval(context) + ")";
  return this->complexity;
}