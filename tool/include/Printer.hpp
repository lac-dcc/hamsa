#ifndef PRINTER
#define PRINTER

#include "Complexity.hpp"
#include "LoopInfoTool.hpp"
#include "Kernel.hpp"
#include "clang/AST/ASTContext.h"
#include <string>

/**
 * \class Printer
 *
 * \brief Abstract class to generate an output file.
 */
class Printer {
public:
  /**
   * \brief Method that generates the output for a specific output format.
   * \param root Kernels tree root.
   * \param Context Clang AST context.
   * \param outName Output file name.
   */
  virtual void gen_out(SeqKernel* root, clang::ASTContext& Context, std::string outName) = 0;

  /**
   * \brief Parse any expression to string.
   * \param expr Expr to be parsed.
   * \param context Clang AST context.
   */
  static std::string getSourceCodeText(clang::Expr* expr, clang::ASTContext& Context);

  /**
   * \brief Parser to LoopKernels increment expression.
   * \param inc LoopKernel increment Expr.
   * \param Context Clang AST context.
   */
  static std::string getIncRepresentation(clang::Expr* inc, clang::ASTContext& Context);
};

/**
 * \class TxtPrinter
 *
 * \brief Concrete Printer class for the .txt format.
 */
class TxtPrinter : public Printer {
public:
  virtual void gen_out(SeqKernel* root, clang::ASTContext& Context, std::string outName);
};

/**
 * \class DotPrinter
 *
 * \brief Concrete Printer class for the .dot format.
 */
class DotPrinter : public Printer {
public:
  virtual void gen_out(SeqKernel* root, clang::ASTContext& Context, std::string outName);
};

/**
 * \class PerfModelPrinter
 *
 * \brief Concrete Printer class for a performance model function.
 */
class PerfModelPrinter : public Printer {
private:
  llvm::SmallVector<TensilicaVar, 4>* tensilicaVariables;

public:
  PerfModelPrinter(llvm::SmallVector<TensilicaVar, 4>* variables) : tensilicaVariables(variables) {}

  virtual void gen_out(SeqKernel* root, clang::ASTContext& Context, std::string outName);
};
#endif