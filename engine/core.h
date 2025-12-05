#pragma once

#include <functional>

namespace engine {

struct EngineConfig {
  // Add configuration options here if needed
};

void init(const EngineConfig &config = {});
void run(std::function<bool()> update_callback);
void shutdown();

} // namespace engine
