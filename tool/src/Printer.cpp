#include "Printer.hpp"
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/Lexer.h"
#include <fstream>
#include <iostream>

using namespace llvm;
using namespace clang;

std::string Printer::getSourceCodeText(Expr* expr, ASTContext& Context) {
  SourceManager& srcManager = Context.getSourceManager();
  const LangOptions& langOpts = Context.getLangOpts();
  return Lexer::getSourceText(CharSourceRange::getTokenRange(expr->getSourceRange()), srcManager, langOpts).str();
}

std::string getIncRepresentation(clang::Expr* inc, ASTContext& Context) {
  if (auto uOp = dyn_cast<UnaryOperator>(inc)) {
    if (uOp->isIncrementOp())
      return "+1";
    if (uOp->isDecrementOp())
      return "-1";
  } else if (auto bOp = dyn_cast<BinaryOperator>(inc)) {
    if (bOp->getOpcodeStr() == "+=")
      return Printer::getSourceCodeText(bOp->getRHS(), Context);
    else if (bOp->getOpcodeStr() == "-=")
      return "-" + Printer::getSourceCodeText(bOp->getRHS(), Context);
    else if (bOp->getOpcodeStr() == "*=")
      return "*" + Printer::getSourceCodeText(bOp->getRHS(), Context);
    else if (bOp->getOpcodeStr() == "/=")
      return "/" + Printer::getSourceCodeText(bOp->getRHS(), Context);
  }
  return Printer::getSourceCodeText(inc, Context);
}
void TextPrinter::gen_out(const DenseMap<int64_t, LoopKernel*>& kernels, const SeqKernel& root, ASTContext& Context, std::string outName) {
  std::fstream outputFile;
  SourceManager& srcManager = Context.getSourceManager();
  outputFile.open("output/" + outName, std::fstream::out);

  for (auto const& [id, kernel] : kernels) {
    outputFile << "at line " << srcManager.getSpellingLineNumber(kernel->begin) << ": "
               << kernel->induc->getNameAsString() << ", <" << Printer::getSourceCodeText(kernel->init, Context) << ", "
               << Printer::getSourceCodeText(kernel->limit, Context) << ", "
               << getIncRepresentation(kernel->inc, Context) << "> [";

    bool isFirst = true;
    for (auto* input : kernel->inputs) {
      outputFile << (isFirst ? isFirst = false, "" : ", ") << input->getNameAsString();
    }

    outputFile << "], O(" << kernel->complexity << ")\n";
  }
}

void DOTPrinter::gen_out(const DenseMap<int64_t, LoopKernel*>& kernels, const SeqKernel& root, ASTContext& Context, std::string outName) {
  std::fstream outputFile;
  SourceManager& srcManager = Context.getSourceManager();
  outputFile.open("output/" + outName, std::fstream::out);

  std::string links = "";
  std::string nodes = "";

  for (auto const& [id, kernel] : kernels) {
    if (kernel->parent != nullptr) {
      nodes += std::to_string((long long int) (kernel->parent)) + "[label=\" Seq \"]\n";
      links += std::to_string((long long int) (kernel->parent)) + " -> " + std::to_string(kernel->id) + "\n";
    }
    nodes += std::to_string(kernel->id) + "[";
    nodes += "label=\"" + kernel->induc->getNameAsString() + ", <" + Printer::getSourceCodeText(kernel->init,
    Context) +
             ", " + Printer::getSourceCodeText(kernel->limit, Context) + ", " +
             getIncRepresentation(kernel->inc, Context) + ">\"]\n";
    if(kernel->child->children.size())
      links += std::to_string(kernel->id) + " -> " + std::to_string((long long int) (&kernel->child)) + "\n";
  }
  outputFile << "digraph {\n" << nodes << '\n' << links << "}";
}
