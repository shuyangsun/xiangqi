# Source: https://github.com/apple/swift-cmake-examples/blob/main/3_bidirectional_cxx_interop/lib/fibonacci/CMakeLists.txt

add_library(xiangqi_board_clib STATIC
    board.c
)

add_library(xiangqi_board_lib STATIC
    board.cc
    internal/moves.cc
)

target_link_libraries(xiangqi_board_lib PRIVATE xiangqi_board_clib)

add_library(xiangqi_game_lib STATIC
    game.cc
    # agent.cc
    # internal/agents/util.cc
    # internal/agents/mcts.cc
    # internal/agents/random.cc
)

target_link_libraries(xiangqi_game_lib PRIVATE xiangqi_board_lib)

set_target_properties(xiangqi_game_lib PROPERTIES Swift_MODULE_NAME "SwiftXiangqiGameLib")
target_compile_options(xiangqi_game_lib PUBLIC
    "$<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default>")
