find_package(Clang REQUIRED) 

include_directories(${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(ForInfoTool main.cpp
                ForInfoTool.cpp)

target_link_libraries(main 
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