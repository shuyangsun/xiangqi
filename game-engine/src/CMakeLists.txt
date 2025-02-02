add_library(xiangqi_engine
    game.cc
    internal/moves.cc
)

target_include_directories(xiangqi_engine PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
