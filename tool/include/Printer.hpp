#ifndef PRINTER
#define PRINTER

#include "Complexity.hpp"
#include "Kernel.hpp"
#include "clang/AST/ASTContext.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/ADT/DenseMap.h"
#include <string>

class Printer {
public:
  virtual void gen_out(const llvm::DenseMap<int64_t, LoopKernel*>& kernels, SeqKernel* root, clang::ASTContext& Context,
                       std::string outName) = 0;
  static std::string getSourceCodeText(clang::Expr* expr, clang::ASTContext& Context);
  static std::string getIncRepresentation(clang::Expr* inc, clang::ASTContext& Context);
};

class TextPrinter : public Printer {
public:
  virtual void gen_out(const llvm::DenseMap<int64_t, LoopKernel*>& kernels, SeqKernel* root, clang::ASTContext& Context,
                       std::string outName);
};

class DOTPrinter : public Printer {
public:
  virtual void gen_out(const llvm::DenseMap<int64_t, LoopKernel*>& kernels, SeqKernel* root, clang::ASTContext& Context,
                       std::string outName);
};
#endif