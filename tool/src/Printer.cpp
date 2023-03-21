#include "Printer.hpp"
#include "KernelVisitor.hpp"
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
    switch (bOp->getOpcode()) {
    case BinaryOperator::Opcode::BO_AddAssign:
      return "+" + Printer::getSourceCodeText(bOp->getRHS(), Context);
    case BinaryOperator::Opcode::BO_SubAssign:
      return "-" + Printer::getSourceCodeText(bOp->getRHS(), Context);
    case BinaryOperator::Opcode::BO_MulAssign:
      return "*" + Printer::getSourceCodeText(bOp->getRHS(), Context);
    case BinaryOperator::Opcode::BO_DivAssign:
      return "/" + Printer::getSourceCodeText(bOp->getRHS(), Context);
    default:
      break;
    }
  }
  return Printer::getSourceCodeText(inc, Context);
}

void TxtPrinter::gen_out(SeqKernel* root, ASTContext& Context, std::string outName) {
  std::fstream outputFile("output/" + outName, std::fstream::out);
  TxtKernelVisitor txtVisitor(&Context);
  outputFile << txtVisitor.visit(root) << "\nTotal complexity: O(" << root->complexity << ")";
}

void DotPrinter::gen_out(SeqKernel* root, ASTContext& Context, std::string outName) {
  std::fstream outputFile("output/" + outName, std::fstream::out);
  DotKernelVisitor visitor(&Context);
  outputFile << "digraph {\n" << visitor.visit(root) << "}";
}

void TensilicaPrinter::gen_out(SeqKernel* root, ASTContext& Context, std::string outName) {
  std::fstream outputFile("output/" + outName, std::fstream::app);
  this->visitor.context = &Context;
  SourceManager& srcManager = Context.getSourceManager();

  outputFile << "# at line " +
                    std::to_string(srcManager.getSpellingLineNumber(this->kernelFunction->getSourceRange().getBegin()))
             << ", " << this->kernelFunction->getNameAsString() << ":\n";
  outputFile << "def perfModel(self):\n";
  for (const auto& [varName, var] : *this->tensilicaVariables) {
    if (var.origin == "inTile") {
      outputFile << "\t" << varName << " = self.io.input[0].dims[" << var.dimIndex << "].dim\n";
    } else if (var.origin == "outTile") {
      outputFile << "\t" << varName << " = self.io.output[0].dims[" << var.dimIndex << "].dim\n";
    } else if (var.origin == "XCHAL_IVPN_SIMD_WIDTH") {
      outputFile << "\t" << varName << " = self.XCHAL_IVPN_SIMD_WIDTH\n";
    } else if (var.origin == "pitch") {
      outputFile << "\t" << varName << " = self.io.input[0].dims[" << var.dimIndex << "].pitch\n";
    }
  }
  outputFile << "\treturn TreePerfModel(self._normalized_name(), ";

  if (this->visitor.visitedFunctions.find(this->kernelFunction->getNameAsString()) !=
      this->visitor.visitedFunctions.end()) {
    outputFile << this->visitor.visitedFunctions[this->kernelFunction->getNameAsString()];
  } else {
    std::string complexity = this->visitor.visit(root);
    this->visitor.visitedFunctions[this->kernelFunction->getNameAsString()] = complexity;
    outputFile << complexity;
  }
  outputFile << ")\n\n";
}
