#pragma once

#include "Engine.h"

namespace nexvora {

/**
 * Thin game facade. Phase 1 routes everything through Engine.
 * Future phases will add race-specific logic here.
 */
class Game {
public:
    static Game& instance();

    bool start();
    void stop();

    Engine& engine() { return Engine::instance(); }

private:
    Game() = default;
};

} // namespace nexvora
