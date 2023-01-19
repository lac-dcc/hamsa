#ifndef KERNEL_VISITOR
#define KERNEL_VISITOR

#include "clang/AST/ASTContext.h"
#include <string>

class Kernel;
class LoopKernel;
class SeqKernel;
class CondKernel;

class KernelVisitor {
public:
  clang::ASTContext* context;

  virtual std::string visit(LoopKernel* kernel) = 0;
  virtual std::string visit(SeqKernel* kernel) = 0;
  virtual std::string visit(CondKernel* kernel) = 0;

protected:
  KernelVisitor(clang::ASTContext* context) : context(context) {}
};

class ComplexityKernelVisitor : public KernelVisitor {
public:
  ComplexityKernelVisitor(clang::ASTContext* context) : KernelVisitor(context) {}

  virtual std::string visit(LoopKernel* kernel);
  virtual std::string visit(SeqKernel* kernel);
  virtual std::string visit(CondKernel* kernel);
};

class TxtKernelVisitor : public KernelVisitor {
public:
  TxtKernelVisitor(clang::ASTContext* context) : KernelVisitor(context) {}

  virtual std::string visit(LoopKernel* kernel);
  virtual std::string visit(SeqKernel* kernel);
  virtual std::string visit(CondKernel* kernel);
};

class DotKernelVisitor : public KernelVisitor {
public:
  DotKernelVisitor(clang::ASTContext* context) : KernelVisitor(context) {}
  
  virtual std::string visit(LoopKernel* kernel);
  virtual std::string visit(SeqKernel* kernel);
  virtual std::string visit(CondKernel* kernel);
};

#endif