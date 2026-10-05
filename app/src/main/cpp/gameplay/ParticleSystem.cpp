#include "ParticleSystem.h"
#include <cmath>
#include <algorithm>

namespace nexvora {

void ParticleSystem::initialize() {
    clear();
}

void ParticleSystem::clear() {
    for (int i = 0; i < MAX_PARTICLES; ++i) {
        m_particles[i].active = false;
    }
    m_activeCount = 0;
    m_next = 0;
}

Particle* ParticleSystem::alloc() {
    for (int i = 0; i < MAX_PARTICLES; ++i) {
        int idx = (m_next + i) % MAX_PARTICLES;
        if (!m_particles[idx].active) {
            m_next = (idx + 1) % MAX_PARTICLES;
            m_particles[idx].active = true;
            return &m_particles[idx];
        }
    }
    return nullptr;
}

void ParticleSystem::update(float dt) {
    m_activeCount = 0;
    for (int i = 0; i < MAX_PARTICLES; ++i) {
        auto& p = m_particles[i];
        if (!p.active) continue;
        p.life -= dt;
        if (p.life <= 0.0f) {
            p.active = false;
            continue;
        }
        p.position += p.velocity * dt;
        p.velocity.y += 1.5f * dt; // slight rise/float
        p.velocity *= (1.0f - 1.5f * dt);
        m_activeCount++;
    }
}

void ParticleSystem::emitExhaust(const math::Vec3& pos, const math::Vec3& dir, bool nitro) {
    int count = nitro ? 4 : 2;
    for (int i = 0; i < count; ++i) {
        Particle* p = alloc();
        if (!p) return;
        p->position = pos;
        p->velocity = dir * (nitro ? -8.0f : -4.0f) + math::Vec3{
            ((i % 3) - 1) * 0.8f, 0.5f + i * 0.2f, ((i % 2) - 0.5f) * 0.6f
        };
        p->life = nitro ? 0.45f : 0.3f;
        p->maxLife = p->life;
        p->size = nitro ? 0.35f : 0.2f;
        p->type = nitro ? ParticleType::Nitro : ParticleType::Exhaust;
        p->color = nitro ? math::Vec3{0.3f, 0.7f, 1.0f} : math::Vec3{0.3f, 0.3f, 0.3f};
    }
}

void ParticleSystem::emitSmoke(const math::Vec3& pos, int count) {
    for (int i = 0; i < count; ++i) {
        Particle* p = alloc();
        if (!p) return;
        p->position = pos;
        p->velocity = {((i%3)-1)*1.5f, 1.0f, ((i%2)-0.5f)*1.2f};
        p->life = 0.6f;
        p->maxLife = 0.6f;
        p->size = 0.4f;
        p->type = ParticleType::Smoke;
        p->color = {0.5f, 0.5f, 0.5f};
    }
}

void ParticleSystem::emitSparks(const math::Vec3& pos, int count) {
    for (int i = 0; i < count; ++i) {
        Particle* p = alloc();
        if (!p) return;
        float a = i / static_cast<float>(count) * math::TWO_PI;
        p->position = pos;
        p->velocity = {std::cos(a)*6.0f, 3.0f + (i%3), std::sin(a)*6.0f};
        p->life = 0.35f;
        p->maxLife = 0.35f;
        p->size = 0.15f;
        p->type = ParticleType::Spark;
        p->color = {1.0f, 0.8f, 0.2f};
    }
}

void ParticleSystem::emitDust(const math::Vec3& pos) {
    for (int i = 0; i < 2; ++i) {
        Particle* p = alloc();
        if (!p) return;
        p->position = pos;
        p->velocity = {((i%2)-0.5f)*2.0f, 0.8f, ((i%3)-1)*1.5f};
        p->life = 0.4f;
        p->maxLife = 0.4f;
        p->size = 0.3f;
        p->type = ParticleType::Dust;
        p->color = {0.55f, 0.5f, 0.35f};
    }
}

} // namespace nexvora
