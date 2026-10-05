#include "Time.h"

namespace nexvora {

float Time::s_deltaTime = 0.0f;
float Time::s_totalTime = 0.0f;
float Time::s_fps = 0.0f;
float Time::s_fpsAccumulator = 0.0f;
int Time::s_fpsFrames = 0;
uint64_t Time::s_frameCount = 0;

void Time::update(float deltaTime) {
    s_deltaTime = deltaTime;
    s_totalTime += deltaTime;
    s_frameCount++;

    s_fpsAccumulator += deltaTime;
    s_fpsFrames++;

    if (s_fpsAccumulator >= 0.5f) {
        s_fps = static_cast<float>(s_fpsFrames) / s_fpsAccumulator;
        s_fpsAccumulator = 0.0f;
        s_fpsFrames = 0;
    }
}

} // namespace nexvora
