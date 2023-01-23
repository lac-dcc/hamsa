#include "KernelVisitor.hpp"
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

std::string Printer::getIncRepresentation(Expr* inc, ASTContext& Context) {
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

void TextPrinter::gen_out(const DenseMap<int64_t, LoopKernel*>& kernels, SeqKernel* root, ASTContext& Context,
                          std::string outName) {
  std::fstream outputFile;
  outputFile.open("output/" + outName, std::fstream::out);
  TxtKernelVisitor txtVisitor(&Context);
  outputFile << txtVisitor.visit(root);
}

void DOTPrinter::gen_out(const DenseMap<int64_t, LoopKernel*>& kernels, SeqKernel* root, ASTContext& Context,
                         std::string outName) {
  std::fstream outputFile;
  outputFile.open("output/" + outName, std::fstream::out);
  DotKernelVisitor visitor(&Context);
  std::string links = visitor.visit(root);
  std::string nodes = "";

  for (auto const& [id, kernel] : kernels) {
    nodes += std::to_string(kernel->id) + "[";
    nodes += "label=\"" + kernel->induc->getNameAsString() + ", <" + Printer::getSourceCodeText(kernel->init, Context) +
             ", " + Printer::getSourceCodeText(kernel->limit, Context) + ", " +
             Printer::getIncRepresentation(kernel->inc, Context) + ">\"]\n";
  }
  outputFile << "digraph {\n" << nodes << '\n' << links << "}";
}
