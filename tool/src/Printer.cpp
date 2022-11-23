#include "Printer.hpp"
#include "clang/Lex/Lexer.h"
#include <fstream>

std::string Printer::getSourceCodeText(clang::Expr* expr, SourceManager& srcManager, LangOptions langOpts) {
  return Lexer::getSourceText(CharSourceRange::getTokenRange(expr->getSourceRange()), srcManager, langOpts).str();
}

void TextPrinter::gen_out(const DenseMap<int64_t, Kernel*>& kernels, ASTContext& Context) {
  SourceManager& srcManager = Context.getSourceManager();
  const LangOptions& langOpts = Context.getLangOpts();
  auto srcLocation = (*kernels.begin()).second->init->getExprLoc();
  auto fileName = srcManager.getFilename(srcLocation);

  for (int i = fileName.size(); i >= 0; i--) {
    if (fileName[i] == '/')
      fileName = fileName.substr(i + 1, fileName.size());
  }

  std::string outputDir = "output/" + fileName.str() + ".out.txt";
  std::fstream outputFile;
  outputFile.open(outputDir, std::fstream::out);

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
