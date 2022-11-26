#ifndef PRINTER
#define PRINTER

#include "LoopInfoTool.hpp"

using namespace clang;

class Printer {
  public:
    virtual void gen_out(const DenseMap<int64_t, Kernel*>& kernels, ASTContext& Context, std::string outName) = 0;
    std::string getSourceCodeText(Expr *expr, SourceManager& srcManager, LangOptions langOpts);
};

class TextPrinter : public Printer {
  public:
    virtual void gen_out(const DenseMap<int64_t, Kernel*>& kernels, ASTContext& Context, std::string outName);
};

#endif