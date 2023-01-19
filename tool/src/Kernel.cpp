#include "Kernel.hpp"
#include "KernelVisitor.hpp"
#include "Printer.hpp"
#include <locale>

using namespace clang;


LoopKernel::LoopKernel(int64_t id) {
  this->child = new SeqKernel;
  this->child->id = id+1;
  this->id = id;
}

LoopKernel::~LoopKernel() {
  delete this->child;
}

CondKernel::CondKernel(int64_t id) {
  this->thenChild = new SeqKernel;
  this->elseChild = new SeqKernel;
  this->id = id;
  this->thenChild->id = id+1;
  this->elseChild->id = id+2;
}

CondKernel::~CondKernel() {
  delete this->thenChild;
  delete this->elseChild;
}

std::string LoopKernel::accept(KernelVisitor* visitor) {
  return visitor->visit(this);
}

std::string SeqKernel::accept(KernelVisitor* visitor) {
  return visitor->visit(this);
}

std::string CondKernel::accept(KernelVisitor* visitor) {
  return visitor->visit(this);
}
