# Source: https://github.com/apple/swift-cmake-examples/blob/main/3_bidirectional_cxx_interop/lib/fibonacci/CMakeLists.txt

add_library(xiangqi STATIC
    game.cc
    internal/moves.cc
)

set_target_properties(xiangqi PROPERTIES Swift_MODULE_NAME "SwiftXiangqi")
target_compile_options(xiangqi PUBLIC
    "$<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default>")
