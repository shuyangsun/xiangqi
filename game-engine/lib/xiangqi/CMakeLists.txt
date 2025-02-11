# Source: https://github.com/apple/swift-cmake-examples/blob/main/3_bidirectional_cxx_interop/lib/fibonacci/CMakeLists.txt

add_library(xiangqi_lib STATIC
    game.cc
    agent.cc
    internal/moves.cc
    internal/agents/mcts.cc
    internal/agents/random.cc
)

set_target_properties(xiangqi_lib PROPERTIES Swift_MODULE_NAME "SwiftXiangqiLib")
target_compile_options(xiangqi_lib PUBLIC
    "$<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default>")
