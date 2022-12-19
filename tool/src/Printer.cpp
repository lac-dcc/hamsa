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
  SourceManager& srcManager = Context.getSourceManager();
  outputFile.open("output/" + outName, std::fstream::out);

  for (auto const& [id, kernel] : kernels) {
    outputFile << "at line " << srcManager.getSpellingLineNumber(kernel->begin) << ": "
               << kernel->induc->getNameAsString() << ", <"
               << Printer::getSourceCodeText(kernel->init, Context) << ", "
               << Printer::getSourceCodeText(kernel->limit, Context) << ", "
               << Printer::getSourceCodeText(kernel->inc, Context) << "> [";

    bool isFirst = true;
    for (auto* input : kernel->inputs) {
      outputFile << (isFirst ? isFirst = false, "" : ", ") << input->getNameAsString();
    }

    outputFile << "], O(" << kernel->complexity  << ")\n";
  }
}

void DOTPrinter::gen_out(const DenseMap<int64_t, Kernel*>& kernels, ASTContext& Context, std::string outName) {
  std::fstream outputFile;
  SourceManager& srcManager = Context.getSourceManager();
  outputFile.open("output/" + outName, std::fstream::out);
  
  std::string links = "";
  std::string nodes = "";

  for (auto const& [id, kernel] : kernels) {
    nodes += std::to_string(kernel->id) + "[label=\""
               + kernel->induc->getNameAsString() + ", <"
               + Printer::getSourceCodeText(kernel->init, Context) + ", "
               + Printer::getSourceCodeText(kernel->limit, Context) + ", "
               + Printer::getSourceCodeText(kernel->inc, Context) + ">\"]\n";

    for(auto child : kernel->children) {
      links += std::to_string(kernel->id) + " -> " + std::to_string(child->id) + "\n";
    }
  }
  outputFile << "digraph {\n" << nodes << '\n' << links << "}";
}
