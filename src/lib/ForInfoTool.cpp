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
        auto inc = fstmt->getInc();
        auto body = fstmt->getBody();

        if (init) {
            // Initialization as assignment expression
            if (auto assign = dyn_cast<BinaryOperator>(init)) {
                if (assign->isAssignmentOp()) {
                    if (auto initVar = dyn_cast<VarDecl>(assign->getLHS()->getReferencedDeclOfCallee()))
                        outs() << initVar->getNameAsString() << " = ";

                    // Initialization with RHS as an integer
                    if (auto initValInt = dyn_cast<IntegerLiteral>(assign->getRHS())) {
                        outs() << (int)initValInt->getValue().roundToDouble() << " ; ";
                    }
                    // Initialization with RHS as another variable
                    else if (auto initDeclRef = dyn_cast<VarDecl>(assign->getRHS()->getReferencedDeclOfCallee())) {
                        outs() << initDeclRef->getNameAsString() << " ; ";
                    }
                }
            }
            // Initialzation with a var declaration
            else if (auto varDeclStmt = dyn_cast<DeclStmt>(init)) {
                if (auto valDecl = dyn_cast<VarDecl>(varDeclStmt->getSingleDecl())) {
                    outs() << valDecl->getType().getAsString() << " " << valDecl->getNameAsString() << " = ";
                    // Initialization with an integer
                    if (auto varDeclInt = dyn_cast<IntegerLiteral>(valDecl->getInit())) {
                        outs() << varDeclInt->getValue() << " ; ";
                    }
                    // Initialization with another variable
                    else if (auto varDeclRef = dyn_cast<VarDecl>(valDecl->getInit()->IgnoreImpCasts()->getReferencedDeclOfCallee())) {
                        outs() << varDeclRef->getNameAsString() << " ; ";
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
                        outs() << condvarL->getNameAsString() << " " << bo->getOpcodeStr().data()
                        << " " << condvarR->getNameAsString() << " ; ";
                    }
                    // For ForCondExpr like "i > 10"
                    else if (auto condvalR = dyn_cast<IntegerLiteral>(boolRHS)) {
                        outs() << condvarL->getNameAsString() << " " << bo->getOpcodeStr().data()
                        << " " << (int) condvalR->getValue().roundToDouble() << " ; ";
                    }
                }
            }
        }

        if (inc) {
            if (auto unaryOp = dyn_cast<UnaryOperator>(inc)) {
                if (unaryOp->isIncrementDecrementOp()) {
                    if (auto varRef = dyn_cast<VarDecl>(unaryOp->getSubExpr()->getReferencedDeclOfCallee())) {
                        if (unaryOp->isPrefix()) outs() << unaryOp->getOpcodeStr(unaryOp->getOpcode());
                        outs() << varRef->getNameAsString();
                        if (unaryOp->isPostfix()) outs() << unaryOp->getOpcodeStr(unaryOp->getOpcode());
                        outs() << '\n';
                    }
                }
            }
        }

        if (body) {
            if (auto bodyStmt = dyn_cast<CompoundStmt>(body)) {
                auto it = bodyStmt->children().begin();
                outs() << "{\n";
                while (it != bodyStmt->children().end()) {
                    if (auto nestedFor = dyn_cast<ForStmt>(*it)) {
                        VisitForStmt(nestedFor);
                    }
                    it++;
                }
                outs() << "}\n";
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