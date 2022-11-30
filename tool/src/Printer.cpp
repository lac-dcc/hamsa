#include "Printer.hpp"
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/Lexer.h"
#include <fstream>

using namespace llvm;
using namespace clang;

std::string Printer::getSourceCodeText(Expr* expr, ASTContext& Context) {
  SourceManager& srcManager = Context.getSourceManager();
  const LangOptions& langOpts = Context.getLangOpts();
  return Lexer::getSourceText(CharSourceRange::getTokenRange(expr->getSourceRange()), srcManager, langOpts).str();
}

void TextPrinter::gen_out(const DenseMap<int64_t, Kernel*>& kernels, ASTContext& Context, std::string outName) {


  std::fstream outputFile;
  outputFile.open("output/" + outName, std::fstream::out);

  for (auto const& [id, kernel] : kernels) {
    outputFile << kernel->induc->getNameAsString() << ", <"
               << Printer::getSourceCodeText(kernel->init, Context) << ", "
               << Printer::getSourceCodeText(kernel->limit, Context) << ", "
               << Printer::getSourceCodeText(kernel->inc, Context) << "> [";

    bool isFirst = true;
    for (auto* input : kernel->inputs) {
      outputFile << (isFirst ? isFirst = false, "" : ", ") << input->getNameAsString();
    }
    outputFile << "], " << kernel->complexity << '\n';
  }
}
