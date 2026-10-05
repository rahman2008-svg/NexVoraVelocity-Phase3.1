#include "Game.h"
#include "Logger.h"

namespace nexvora {

Game& Game::instance() {
    static Game g;
    return g;
}

bool Game::start() {
    NV_LOGI("Game::start (Phase 3 Racing)");
    return Engine::instance().initialize();
}

void Game::stop() {
    NV_LOGI("Game::stop");
    Engine::instance().shutdown();
}

} // namespace nexvora
