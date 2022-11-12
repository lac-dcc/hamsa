find_package(Clang REQUIRED) 

include_directories(${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(LoopInfoTool main.cpp
                LoopInfoTool.cpp)

target_link_libraries(LoopInfoTool
    clangAST
    clangASTMatchers
    clangBasic
    clangDriver
    clangFrontend
    clangLex
    clangRewriteFrontend
    clangSerialization
    clangStaticAnalyzerFrontend
    clangTooling
    clangToolingSyntax
    LLVMSupport
)