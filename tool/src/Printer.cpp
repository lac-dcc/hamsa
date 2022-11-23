#include "Printer.hpp"
#include "clang/Lex/Lexer.h"
#include <fstream>

std::string Printer::getSourceCodeText(clang::Expr* expr, SourceManager& srcManager, LangOptions langOpts) {
  return Lexer::getSourceText(CharSourceRange::getTokenRange(expr->getSourceRange()), srcManager, langOpts).str();
}

void TextPrinter::gen_out(const DenseMap<int64_t, Kernel*>& kernels, ASTContext& Context, std::string outName) {
  SourceManager& srcManager = Context.getSourceManager();
  const LangOptions& langOpts = Context.getLangOpts();
  auto fileName = outName;

  for (int i = outName.length() - 1; i >= 0; i--) {
    if (outName[i] == '/') {
      fileName = outName.substr(i + 1, outName.size() - i);
      break;
    }
  }

  std::fstream outputFile;
  outputFile.open("output/" + fileName + ".out.txt", std::fstream::out);

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
