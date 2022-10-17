find_package(Clang REQUIRED)

include_directories(${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})

add_definitions(${LLVM_DEFINITIONS})

add_executable(ForDependencies ForDependencies.cpp)

target_link_libraries(ForDependencies 
    clangAST
    clangASTMatchers
    clangBasic
    clangDriver
    clangFrontend
    clangRewriteFrontend
    clangSerialization
    clangStaticAnalyzerFrontend
    clangTooling
    clangToolingSyntax
    LLVMSupport
)