find_package(Clang REQUIRED) 

include_directories(${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_library(LoopInfoTool SHARED LoopInfoTool.cpp KernelVisitor.cpp Kernel.cpp Printer.cpp Complexity.cpp)

set_target_properties(LoopInfoTool PROPERTIES
    LINKER_LANGUAGE CXX COMPILE_FLAGS "-fno-rtti")