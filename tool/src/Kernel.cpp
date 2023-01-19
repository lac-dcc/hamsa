#include "Kernel.hpp"
#include "KernelVisitor.hpp"
#include "Printer.hpp"
#include <locale>

using namespace clang;


LoopKernel::LoopKernel(int64_t id) {
  this->child = new SeqKernel;
  this->child->id = id+1;
  this->id = id;
}

LoopKernel::~LoopKernel() {
  delete this->child;
}

CondKernel::CondKernel(int64_t id) {
  this->thenChild = new SeqKernel;
  this->elseChild = new SeqKernel;
  this->id = id;
  this->thenChild->id = id+1;
  this->elseChild->id = id+2;
}

CondKernel::~CondKernel() {
  delete this->thenChild;
  delete this->elseChild;
}

std::string LoopKernel::accept(KernelVisitor* visitor) {
  return visitor->visit(this);
}

std::string SeqKernel::accept(KernelVisitor* visitor) {
  return visitor->visit(this);
}

std::string CondKernel::accept(KernelVisitor* visitor) {
  return visitor->visit(this);
}

// std::string getIncRepresentation(Expr* inc, ASTContext& Context) {
//   if (auto uOp = dyn_cast<UnaryOperator>(inc)) {
//     if (uOp->isIncrementOp())
//       return "+1";
//     if (uOp->isDecrementOp())
//       return "-1";
//   } else if (auto bOp = dyn_cast<BinaryOperator>(inc)) {
//     if (bOp->getOpcodeStr() == "+=")
//       return Printer::getSourceCodeText(bOp->getRHS(), Context);
//     else if (bOp->getOpcodeStr() == "-=")
//       return "-" + Printer::getSourceCodeText(bOp->getRHS(), Context);
//     else if (bOp->getOpcodeStr() == "*=")
//       return "*" + Printer::getSourceCodeText(bOp->getRHS(), Context);
//     else if (bOp->getOpcodeStr() == "/=")
//       return "/" + Printer::getSourceCodeText(bOp->getRHS(), Context);
//   }

//   return Printer::getSourceCodeText(inc, Context);
// }

// std::string LoopKernel::printTxt(ASTContext& context) {
//   SourceManager& srcManager = context.getSourceManager();
//   std::string out = "at line " + std::to_string(srcManager.getSpellingLineNumber(this->begin)) + ": "
//                + this->induc->getNameAsString() + ", <" + Printer::getSourceCodeText(this->init, context) + ", "
//                + Printer::getSourceCodeText(this->limit, context) + ", "
//                + getIncRepresentation(this->inc, context) + "> [";
  
//   bool isFirst = true;
//   for (auto* input : this->inputs) {
//     out += (isFirst ? isFirst = false, "" : ", ") + input->getNameAsString();
//   }

//   out += "], O(" + this->complexity + ")\n";


//   return out + child.printTxt(context);
// }

// std::string SeqKernel::printTxt(ASTContext& context) {
//   std::string out = "";
//   for (Kernel* child : this->children) {
//     out += child->printTxt(context);
//   }

//   return out;
// }

// std::string CondKernel::printTxt(ASTContext& context) {
//   return leftChild.printTxt(context) + rightChild.printTxt(context);
// }