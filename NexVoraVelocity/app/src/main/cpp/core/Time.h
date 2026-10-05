#pragma once

#include <cstdint>

namespace nexvora {

class Time {
public:
    static void update(float deltaTime);

    static float deltaTime() { return s_deltaTime; }
    static float totalTime() { return s_totalTime; }
    static float fps() { return s_fps; }
    static uint64_t frameCount() { return s_frameCount; }

private:
    static float s_deltaTime;
    static float s_totalTime;
    static float s_fps;
    static float s_fpsAccumulator;
    static int s_fpsFrames;
    static uint64_t s_frameCount;
};

} // namespace nexvora
