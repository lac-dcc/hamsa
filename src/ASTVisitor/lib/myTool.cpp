#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"

using namespace clang;
using namespace clang::tooling;
using namespace llvm;

static llvm::cl::OptionCategory MatcherCategory("matcher options");

class FindForCondVisitor : public RecursiveASTVisitor<FindForCondVisitor>
{
public:
    explicit FindForCondVisitor(ASTContext *Context)
        : Context(Context) {}

    bool VisitForStmt(ForStmt *fstmt)
    {      
        fstmt->dumpColor();
        auto cond = fstmt->getCond();
        if(cond)
        {   
            if(auto bo = dyn_cast<BinaryOperator>(cond))
            {     
                auto boolRHS = bo->getRHS();
                auto boolLHS = bo->getLHS();
                if(auto condvarL = dyn_cast<VarDecl>(boolLHS->getReferencedDeclOfCallee()))
                {   
                //For ForCondExpr like "i < n"
                if(auto condvarR = dyn_cast<VarDecl>(boolRHS->getReferencedDeclOfCallee()))
                {   
                    printf("%s %s %s\n",condvarL->getNameAsString().c_str(),
                                        bo->getOpcodeStr().data(),
                                        condvarR->getNameAsString().c_str());
                }
                //For ForCondExpr like "i > 10"
                else if(auto condvalR = dyn_cast<IntegerLiteral>(boolRHS))
                {
                    printf("%s %s %0.0lf\n",condvarL->getNameAsString().c_str(),
                                            bo->getOpcodeStr().data(),
                                            condvalR->getValue().roundToDouble());
                }
                }
            }
        }
        return true;
    }
//    bool VisitDeclRefExpr(DeclRefExpr *sta)
//     {   
//         FullSourceLoc FullLocation = Context->getFullLoc(sta->getBeginLoc());
//         SourceManager &srcMgr = Context->getSourceManager();
//         llvm::outs() << Context->getObjCClassType();
//         if ( FullLocation.isValid() )
//         {
//             // Print function or variable name
//             printf("stm: %-23s at %3u:%-3u in %-15s\n",
//                 (sta->getFoundDecl())->getNameAsString().c_str(),
//                 FullLocation.getSpellingLineNumber(),
//                 FullLocation.getSpellingColumnNumber(),
//                 srcMgr.getFilename(FullLocation).data());
//         }
//         return true;
//     }

private:
    ASTContext *Context;
};

class FindForCondConsumer : public clang::ASTConsumer
{
public:
    explicit FindForCondConsumer(ASTContext *Context)
        : Visitor(Context) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context)
    {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    FindForCondVisitor Visitor;
};

class FindForCondAction : public clang::ASTFrontendAction
{
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
        clang::CompilerInstance &Compiler, llvm::StringRef InFile)
    {
        return std::make_unique<FindForCondConsumer>(&Compiler.getASTContext());
    }
};

int main(int argc, const char **argv)
{   
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, MatcherCategory);
    if (!ExpectedParser) {
        // Fail gracefully for unsupported options.
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }
    CommonOptionsParser& OptionsParser = ExpectedParser.get();

    ClangTool Tool(OptionsParser.getCompilations(), 
                    OptionsParser.getSourcePathList());

    return Tool.run(clang::tooling::newFrontendActionFactory<FindForCondAction>().get());
}