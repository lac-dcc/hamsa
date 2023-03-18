#include "KernelVisitor.hpp"
#include "Complexity.hpp"
#include "Printer.hpp"
#include "clang/Basic/SourceManager.h"

using namespace clang;

std::string ComplexityKernelVisitor::visit(LoopKernel* kernel) {
  kernel->complexity = calculateSingleCost(kernel, *this->context) + "*" + this->visit(kernel->child);
  if (kernel->complexity[kernel->complexity.size() - 1] == '*')
    kernel->complexity.pop_back();
  return kernel->complexity;
}

std::string ComplexityKernelVisitor::visit(SeqKernel* kernel) {
  kernel->complexity = "";
  for (Kernel* child : kernel->children) {
    kernel->complexity += child->accept(this) + "+";
  }

  if (kernel->complexity.size() > 0)
    kernel->complexity.pop_back();

  if (kernel->children.size() >= 2)
    kernel->complexity = "(" + kernel->complexity + ")";

  return kernel->complexity;
}

std::string ComplexityKernelVisitor::visit(CondKernel* kernel) {
  kernel->complexity = "(" + this->visit(kernel->thenChild) + " | ";

  if (kernel->elseChild != nullptr && kernel->elseChild->children.size() > 0)
    kernel->complexity += this->visit(kernel->elseChild) + ")";
  else
    kernel->complexity += "1)";

  return kernel->complexity;
}

std::string TxtKernelVisitor::visit(LoopKernel* kernel) {
  SourceManager& srcManager = this->context->getSourceManager();
  std::string out = "at line " + std::to_string(srcManager.getSpellingLineNumber(kernel->begin)) + ": " +
                    kernel->induc->getNameAsString() + ", <" +
                    Printer::getSourceCodeText(kernel->init, *this->context) + ", " +
                    Printer::getSourceCodeText(kernel->limit, *this->context) + ", " +
                    Printer::getIncRepresentation(kernel->inc, *this->context) + "> [";

  bool isFirst = true;
  for (auto* input : kernel->inputs) {
    out += (isFirst ? isFirst = false, "" : ", ") + input->getNameAsString();
  }

  out += "], O(" + kernel->complexity + ")\n";

  return out + this->visit(kernel->child);
}

std::string TxtKernelVisitor::visit(SeqKernel* kernel) {
  std::string out = "";
  for (Kernel* child : kernel->children) {
    out += child->accept(this);
  }

  return out;
}

std::string TxtKernelVisitor::visit(CondKernel* kernel) {
  std::string out =
      "if " + Printer::getSourceCodeText(kernel->condition, *this->context) + "\n\t" + this->visit(kernel->thenChild);
  if (kernel->elseChild != nullptr && kernel->elseChild->children.size() > 0)
    out += "else\n\t" + this->visit(kernel->elseChild);

  return out;
}

std::string DotKernelVisitor::visit(LoopKernel* kernel) {
  std::string link = "";
  std::string label = "";
  std::string identifier = std::to_string(kernel->id);

  if (kernel->child->children.size() > 0)
    link += identifier + " -> " + std::to_string(kernel->child->id) + "\n" + this->visit(kernel->child);

  label += identifier + "[label=\"" + kernel->induc->getNameAsString() + ", <" +
           Printer::getSourceCodeText(kernel->init, *this->context) + ", " +
           Printer::getSourceCodeText(kernel->limit, *this->context) + ", " +
           Printer::getIncRepresentation(kernel->inc, *this->context) + ">\"]\n";
  return link + label;
}

std::string DotKernelVisitor::visit(SeqKernel* kernel) {
  std::string links = "";
  std::string identifier = std::to_string(kernel->id);
  std::string label = identifier + "[label=\"Seq\"]\n";
  for (auto child : kernel->children) {
    links += identifier + " -> " + std::to_string(child->id) + "\n" + child->accept(this);
  }

  return label + links;
}

std::string DotKernelVisitor::visit(CondKernel* kernel) {
  std::string links = "";
  std::string label = std::to_string(kernel->id) + "[shape=diamond, label=\"" +
                      Printer::getSourceCodeText(kernel->condition, *this->context) + "\"]\n";

  if (kernel->thenChild->children.size() > 0) {
    links += std::to_string(kernel->id) + " -> " + std::to_string(kernel->thenChild->id) + "\n" +
             this->visit(kernel->thenChild);
  }
  if (kernel->elseChild != nullptr && kernel->elseChild->children.size() > 0) {
    links += std::to_string(kernel->id) + " -> " + std::to_string(kernel->elseChild->id) + "\n" +
             this->visit(kernel->elseChild);
  }
  return label + links;
}

std::string TensilicaKernelVisitor::visit(SeqKernel* kernel) {
  if (kernel->children.size() == 1) {
    return (*kernel->children.begin())->accept(this);
  } else if (kernel->children.size() > 1) {
    std::string out = "";

    if (!this->TPContext.empty() && this->TPContext.top() == this->TPLoops) {
      out += "], ";
    }
    this->TPContext.push(this->TPSeq);

    out += "TPSeq(";
    for (auto child : kernel->children) {
      out += child->accept(this) + ",";
    }
    out.pop_back();

    this->TPContext.pop();
    return out + ")";
  }

  return "";
}

std::string TensilicaKernelVisitor::visit(LoopKernel* kernel) {
  std::string out = "";
  bool openParenthesis = false;
  if (this->TPContext.empty() || this->TPContext.top() != this->TPLoops) {
    out += "TPLoops([";
    this->TPContext.push(this->TPLoops);
    openParenthesis = true;
  } else {
    out += ',';
  }

  out += calculateSingleCost(kernel, *this->context);

  if (kernel->child->children.size() == 0) {
    out += ']';
  } else {
    out += this->visit(kernel->child);
  }

  if (openParenthesis) {
    this->TPContext.pop();
    out += ')';
  }
  return out;
}

std::string TensilicaKernelVisitor::visit(CondKernel* kernel) {
  std::string out = "";
  if (!this->TPContext.empty() && this->TPContext.top() == this->TPLoops) {
    out += "], ";
  }

  this->TPContext.push(this->TPCond);
  out += "TPCond(";

  // Cadence specific
  if (auto* binaryOp = dyn_cast<BinaryOperator>(kernel->condition)) {
    std::string LHS = Printer::getSourceCodeText(binaryOp->getLHS(), *this->context); 
    std::string RHS = Printer::getSourceCodeText(binaryOp->getRHS(), *this->context);

    if(LHS.size() >= 25 && LHS.substr(20, 5) == "PITCH") 
      LHS = "pitch";
      
    switch (binaryOp->getOpcode()) {
    case BinaryOperator::Opcode::BO_LT: 
      out += "cneq(" + RHS + " % 2, 0)";
      break;
    case BinaryOperator::Opcode::BO_GT:
      out += "cgreater(" + LHS + ", " + RHS + ")";
      break;
    case BinaryOperator::Opcode::BO_EQ:
      out += "ceq(" + LHS + ", " + RHS + ")";
      break;
    case BinaryOperator::Opcode::BO_NE:
      out += "cneq(" + LHS + ", " + RHS + ")";
      break;
    case BinaryOperator::Opcode::BO_LE:
      out += "cleq(" + LHS + ", " + RHS + ")";
      break;
    default:
      out += Printer::getSourceCodeText(binaryOp, *this->context);
      break;
    }
  }
  if (kernel->thenChild->children.size() > 0)
   out += ", " +  this->visit(kernel->thenChild);
  out += ")";

  this->TPContext.pop();

  return out;
}

std::string TensilicaKernelVisitor::visit(CallKernel* kernel) {
  llvm::outs() << "Entrou Visit\n";
  if (this->visitedFunctions.find(kernel->kernelName) != this->visitedFunctions.end()) {
    return this->visitedFunctions[kernel->kernelName];
  } else {
    std::string complexity = this->visit(kernel->origin);
    this->visitedFunctions[kernel->kernelName] = complexity;
    return complexity;
  }
}