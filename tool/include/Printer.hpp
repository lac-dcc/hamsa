#ifndef PRINTER
#define PRINTER
#include "LoopInfoTool.hpp"

class Printer {
  public:
    virtual void gen_out(const DenseMap<int64_t, Kernel*>& kernels, ASTContext& Context, std::string outName) = 0;
    std::string getSourceCodeText(clang::Expr *expr, SourceManager& srcManager, LangOptions langOpts);
};

class TextPrinter : public Printer {
  public:
    virtual void gen_out(const DenseMap<int64_t, Kernel*>& kernels, ASTContext& Context, std::string outName);
};

#endif