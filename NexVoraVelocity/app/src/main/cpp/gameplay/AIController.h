#pragma once

#include "VehicleController.h"
#include "Track.h"
#include "RaceState.h"
#include <vector>

namespace nexvora {

class AIController {
public:
    void initialize(VehicleController* vehicle, const Track* track, Difficulty diff, int aiIndex);
    void update(float dt);

private:
    VehicleController* m_vehicle = nullptr;
    const Track* m_track = nullptr;
    Difficulty m_diff = Difficulty::Normal;
    int m_aiIndex = 0;
    int m_targetWp = 0;
    float m_reactionTimer = 0.0f;
    float m_speedMul = 1.0f;
    float m_steerSmooth = 0.0f;
};

} // namespace nexvora
