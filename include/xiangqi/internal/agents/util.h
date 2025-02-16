#ifndef XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_INTERNAL_AGENTS_UTIL_H_
#define XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_INTERNAL_AGENTS_UTIL_H_

#include <random>

namespace xq::internal::agent::util {

std::mt19937& GetRNG();

}  // namespace xq::internal::agent::util

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_INTERNAL_AGENTS_UTIL_H_
