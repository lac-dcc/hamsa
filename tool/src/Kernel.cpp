#include "Kernel.hpp"
#include "KernelVisitor.hpp"

using namespace clang;

LoopKernel::LoopKernel(int64_t id) {
  this->child = new SeqKernel;
  this->child->id = id + 1;
  this->id = id;
}

LoopKernel::~LoopKernel() { delete this->child; }

CondKernel::CondKernel(int64_t id, bool hasElse) {
  this->id = id;
  this->thenChild = new SeqKernel;
  this->thenChild->id = id + 1;
  if (hasElse) {
    this->elseChild = new SeqKernel;
    this->elseChild->id = id + 2;
  } else {
    this->elseChild = nullptr;
  }
}

CondKernel::~CondKernel() {
  delete this->thenChild;
  if (this->elseChild != nullptr)
    delete this->elseChild;
}

std::string LoopKernel::accept(KernelVisitor<std::string>* visitor) { return visitor->visit(this); }

std::string SeqKernel::accept(KernelVisitor<std::string>* visitor) { return visitor->visit(this); }

std::string CondKernel::accept(KernelVisitor<std::string>* visitor) { return visitor->visit(this); }
