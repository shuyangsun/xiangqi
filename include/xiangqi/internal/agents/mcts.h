#ifndef XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_INTERNAL_AGENTS_MCTS_H_
#define XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_INTERNAL_AGENTS_MCTS_H_

#include "xiangqi/agent.h"

namespace xq::internal::agent {

class MCTS : public IAgent {
 public:
  MCTS() = delete;

  MCTS(size_t num_simulations, size_t depth, float exploration_constant);

  ~MCTS() = default;

  virtual uint16_t MakeMove(const Board<Piece>& board,
                            Player player) const override final;

 private:
  const size_t num_iter_;
  const size_t depth_;
  const float exploration_constant_;
};

}  // namespace xq::internal::agent

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_INTERNAL_AGENTS_MCTS_H_
