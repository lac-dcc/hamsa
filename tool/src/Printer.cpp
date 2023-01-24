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

void TxtPrinter::gen_out(SeqKernel* root, ASTContext& Context, std::string outName) {
  std::fstream outputFile;
  outputFile.open("output/" + outName, std::fstream::out);
  TxtKernelVisitor txtVisitor(&Context);
  outputFile << txtVisitor.visit(root) << "\nTotal complexity: O(" << root->complexity << ")";
}

void DotPrinter::gen_out(SeqKernel* root, ASTContext& Context, std::string outName) {
  std::fstream outputFile;
  outputFile.open("output/" + outName, std::fstream::out);
  DotKernelVisitor visitor(&Context);
  outputFile << "digraph {\n" << visitor.visit(root) << "}";
}
