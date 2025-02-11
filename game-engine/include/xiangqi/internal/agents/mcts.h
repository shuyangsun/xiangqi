#ifndef XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_INTERNAL_AGENTS_MCTS_H_
#define XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_INTERNAL_AGENTS_MCTS_H_

#include "xiangqi/agent.h"

namespace xq::internal::agent {

class MCTS : public IAgent {
 public:
  MCTS() = delete;

  MCTS(size_t num_simulations);

  ~MCTS() = default;

  virtual uint16_t MakeMove(const Board<Piece>& board,
                            Player player) const override final;

 private:
  const size_t num_simulations_;
};

}  // namespace xq::internal::agent

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_INTERNAL_AGENTS_MCTS_H_
