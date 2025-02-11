#ifndef XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_INTERNAL_AGENTS_RANDOM_H_
#define XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_INTERNAL_AGENTS_RANDOM_H_

#include "xiangqi/agent.h"

namespace xq::internal::agent {

class Random : public IAgent {
 public:
  Random() = default;
  ~Random() = default;

  virtual Position MakeMove(const Board<Piece>& board,
                            Player player) const override final;
};

}  // namespace xq::internal::agent

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_INTERNAL_AGENTS_RANDOM_H_
