#include "Printer.hpp"
#include "clang/Lex/Lexer.h"
#include <fstream>

std::string Printer::getSourceCodeText(clang::Expr *expr, SourceManager &srcManager, LangOptions langOpts) {
    return Lexer::getSourceText(CharSourceRange::getTokenRange(expr->getSourceRange()), srcManager, langOpts).str();
}


void TextPrinter::gen_out(DenseMap<int64_t, Kernel*> kernels, ASTContext &Context) {
  SourceManager& srcManager = Context.getSourceManager();
  const LangOptions& langOpts = Context.getLangOpts();
  auto srcLocation = (*kernels.begin()).second->init->getExprLoc();
  auto fileName = srcManager.getFilename(srcLocation);
  
  for(int i = fileName.size(); i >= 0; i--) {
    if(fileName[i] == '/') {
      fileName = fileName.substr(i+1, fileName.size());
    }
  }

  std::string outputDir = "output/"+fileName.str()+".out.txt";
  std::fstream outputFile;
  outputFile.open(outputDir, std::fstream::out);

  for (auto k : kernels) {
    auto second = k.second;

    outputFile << second->induc->getNameAsString() << ", <" 
    << this->getSourceCodeText(second->init, srcManager, langOpts)
    << ", "
    << this->getSourceCodeText(second->limit, srcManager, langOpts)
    << ", " 
    << this->getSourceCodeText(second->inc, srcManager, langOpts)
    << "> [";

    for(auto it = second->inputs.begin(); it != second->inputs.end(); it++) {
      if(it != second->inputs.begin())
        outputFile << ", ";
      outputFile << (*it)->getNameAsString();
    }
    outputFile << "]\n";
  }
}
