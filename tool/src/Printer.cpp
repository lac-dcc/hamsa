#include "KernelVisitor.hpp"
#include "Printer.hpp"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Lexer.h"
#include <fstream>
#include <string>

using namespace llvm;
using namespace clang;

std::string Printer::getSourceCodeText(Expr* expr, ASTContext& Context) {
  SourceManager& srcManager = Context.getSourceManager();
  const LangOptions& langOpts = Context.getLangOpts();
  return Lexer::getSourceText(CharSourceRange::getTokenRange(expr->getSourceRange()), srcManager, langOpts).str();
}

std::string Printer::getIncRepresentation(Expr* inc, ASTContext& Context) {
  if (auto* uOp = dyn_cast<UnaryOperator>(inc)) {
    if (uOp->isIncrementOp())
      return "+1";
    if (uOp->isDecrementOp())
      return "-1";
  } else if (auto* bOp = dyn_cast<BinaryOperator>(inc)) {
    auto opCode = bOp->getOpcodeStr();
    if (opCode == "+=")
      return "+" + Printer::getSourceCodeText(bOp->getRHS(), Context);
    else if (opCode == "-=")
      return "-" + Printer::getSourceCodeText(bOp->getRHS(), Context);
    else if (opCode == "*=")
      return "*" + Printer::getSourceCodeText(bOp->getRHS(), Context);
    else if (opCode == "/=")
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

void PerfModelPrinter::gen_out(SeqKernel* root, ASTContext& Context, std::string outName) {
  std::fstream outputFile;
  PerfModelKernelVisitor visitor(&Context);
  outputFile.open("output/" + outName, std::fstream::out);
  outputFile << "def perfModel(self):\n";
  for (const auto& var : *this->tensilicaVariables) {
    if (var.origin == "inTile") {
      outputFile << "\t" << var.name << " = self.io.input[0].dims[" << var.dimIndex << "].dim\n";
    } else if (var.origin == "outTile") {
      outputFile << "\t" << var.name << " = self.io.output[0].dims[" << var.dimIndex << "].dim\n";
    } else if (var.origin == "XCHAL_IVPN_SIMD_WIDTH") {
      outputFile << "\t" << var.name << " = self.XCHAL_IVPN_SIMD_WIDTH\n";
    }
  }
  outputFile << "\treturn TreePerfModel(self._normalized_name(), ";
  outputFile << visitor.visit(root);
  outputFile << ')';
}
