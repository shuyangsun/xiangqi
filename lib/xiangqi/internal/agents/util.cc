#include "xiangqi/internal/agents/util.h"

#include <chrono>
#include <random>

namespace xq::internal::agent::util {

std::mt19937& GetRNG() {
  static std::mt19937 rng(static_cast<unsigned int>(
      std::chrono::steady_clock::now().time_since_epoch().count()));
  return rng;
}

}  // namespace xq::internal::agent::util
