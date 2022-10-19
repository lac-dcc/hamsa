#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;
using namespace clang::tooling;
using namespace llvm;

static llvm::cl::OptionCategory MatcherCategory("matcher options");

class FindForCondVisitor : public RecursiveASTVisitor<FindForCondVisitor> {
   public:
    explicit FindForCondVisitor(ASTContext *Context)
        : Context(Context) {}

    bool VisitForStmt(ForStmt *fstmt) {
        auto init = fstmt->getInit();
        auto cond = fstmt->getCond();

        if (init) {
            // Initialization as assignment expression
            if (auto assign = dyn_cast<BinaryOperator>(init)) {
                if (assign->isAssignmentOp()) {
                    if (auto initVar = dyn_cast<VarDecl>(assign->getLHS()->getReferencedDeclOfCallee()))
                        outs() << initVar->getType().getAsString() << " " << initVar->getNameAsString() << " = ";
                    
                    // Initialization with RHS as another variable
                    if (auto initValDecl = dyn_cast<VarDecl>(assign->getRHS()->getReferencedDeclOfCallee())) {
                        outs() << initValDecl->getNameAsString() << '\n';
                    }
                    // Initialization with RHS as an integer
                    else if (auto initValInt = dyn_cast<IntegerLiteral>(assign->getRHS())) {
                        outs() << (int)initValInt->getValue().roundToDouble() << '\n';
                    }
                }
            }
        }

        if (cond) {
            if (auto bo = dyn_cast<BinaryOperator>(cond)) {
                auto boolRHS = bo->getRHS();
                auto boolLHS = bo->getLHS();
                if (auto condvarL = dyn_cast<VarDecl>(boolLHS->getReferencedDeclOfCallee())) {
                    // For ForCondExpr like "i < n"
                    if (auto condvarR = dyn_cast<VarDecl>(boolRHS->getReferencedDeclOfCallee())) {
                        printf("%s %s %s\n", condvarL->getNameAsString().c_str(),
                               bo->getOpcodeStr().data(),
                               condvarR->getNameAsString().c_str());
                    }
                    // For ForCondExpr like "i > 10"
                    else if (auto condvalR = dyn_cast<IntegerLiteral>(boolRHS)) {
                        printf("%s %s %0.0lf\n", condvarL->getNameAsString().c_str(),
                               bo->getOpcodeStr().data(),
                               condvalR->getValue().roundToDouble());
                    }
                }
            }
        }
        return true;
    }
   private:
    ASTContext *Context;
};

class FindForCondConsumer : public clang::ASTConsumer {
   public:
    explicit FindForCondConsumer(ASTContext *Context)
        : Visitor(Context) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context) {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

   private:
    FindForCondVisitor Visitor;
};

class FindForCondAction : public clang::ASTFrontendAction {
   public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
        clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
        return std::make_unique<FindForCondConsumer>(&Compiler.getASTContext());
    }
};

int main(int argc, const char **argv) {
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, MatcherCategory);
    if (!ExpectedParser) {
        // Fail gracefully for unsupported options.
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }
    CommonOptionsParser &OptionsParser = ExpectedParser.get();

    ClangTool Tool(OptionsParser.getCompilations(),
                   OptionsParser.getSourcePathList());

    return Tool.run(clang::tooling::newFrontendActionFactory<FindForCondAction>().get());
}