#include "xiangqi/agent.h"

#include <memory>

#include "xiangqi/internal/agents/mcts.h"
#include "xiangqi/internal/agents/random.h"

namespace xq {

std::unique_ptr<IAgent> AgentFactory::Random() {
  return std::make_unique<xq::internal::agent::Random>();
}

std::unique_ptr<IAgent> AgentFactory::MCTS(size_t num_simulations) {
  return std::make_unique<xq::internal::agent::MCTS>(num_simulations);
}

}  // namespace xq
