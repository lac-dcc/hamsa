#include "Printer.hpp"
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/Lexer.h"
#include <fstream>

using namespace llvm;
using namespace clang;

std::string Printer::getSourceCodeText(Expr* expr, SourceManager& srcManager, LangOptions langOpts) {
  return Lexer::getSourceText(CharSourceRange::getTokenRange(expr->getSourceRange()), srcManager, langOpts).str();
}

void TextPrinter::gen_out(const DenseMap<int64_t, Kernel*>& kernels, ASTContext& Context, std::string outName) {
  SourceManager& srcManager = Context.getSourceManager();
  const LangOptions& langOpts = Context.getLangOpts();

  std::fstream outputFile;
  outputFile.open("output/" + outName, std::fstream::out);

  for (auto const& [id, kernel] : kernels) {
    outputFile << kernel->induc->getNameAsString() << ", <"
               << this->getSourceCodeText(kernel->init, srcManager, langOpts) << ", "
               << this->getSourceCodeText(kernel->limit, srcManager, langOpts) << ", "
               << this->getSourceCodeText(kernel->inc, srcManager, langOpts) << "> [";

    bool isFirst = true;
    for (auto* input : kernel->inputs) {
      outputFile << (isFirst ? isFirst = false, "" : ", ") << input->getNameAsString();
    }
    outputFile << "]\n";
  }
}
