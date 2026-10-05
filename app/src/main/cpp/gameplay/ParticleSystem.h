#pragma once

#include "Math.h"
#include <vector>
#include <cstdint>

namespace nexvora {

enum class ParticleType : uint8_t {
    Exhaust = 0,
    Nitro = 1,
    Smoke = 2,
    Spark = 3,
    Dust = 4
};

struct Particle {
    math::Vec3 position;
    math::Vec3 velocity;
    math::Vec3 color;
    float life = 0.0f;
    float maxLife = 1.0f;
    float size = 0.2f;
    ParticleType type = ParticleType::Exhaust;
    bool active = false;
};

class ParticleSystem {
public:
    static constexpr int MAX_PARTICLES = 256;

    void initialize();
    void update(float dt);
    void clear();

    void emitExhaust(const math::Vec3& pos, const math::Vec3& dir, bool nitro);
    void emitSmoke(const math::Vec3& pos, int count = 3);
    void emitSparks(const math::Vec3& pos, int count = 8);
    void emitDust(const math::Vec3& pos);

    const Particle* particles() const { return m_particles; }
    int maxParticles() const { return MAX_PARTICLES; }
    int activeCount() const { return m_activeCount; }

private:
    Particle* alloc();
    Particle m_particles[MAX_PARTICLES];
    int m_activeCount = 0;
    int m_next = 0;
};

} // namespace nexvora
