#pragma once

#include "Math.h"
#include <string>

namespace nexvora {

struct VehicleConfig {
    std::string id;
    std::string name;
    math::Vec3 bodyColor{0.85f, 0.15f, 0.12f};
    math::Vec3 accentColor{0.95f, 0.95f, 0.95f};

    float maxSpeed = 48.0f;          // m/s ~173 km/h
    float acceleration = 22.0f;
    float brakeForce = 35.0f;
    float reverseSpeed = 12.0f;
    float steeringStrength = 2.4f;
    float steeringAtHighSpeed = 0.45f; // multiplier at max speed
    float grip = 0.88f;
    float driftFactor = 0.35f;
    float friction = 4.5f;
    float drag = 0.35f;
    float gravity = 18.0f;
    float nitroAcceleration = 35.0f;
    float nitroMaxSpeedBonus = 18.0f;
    float nitroCapacity = 3.5f;      // seconds of full nitro
    float nitroRechargeRate = 0.25f; // per second when not using
    float mass = 1.0f;
    float turnResponsiveness = 1.0f;

    static VehicleConfig velocityX() {
        VehicleConfig c;
        c.id = "velocity_x";
        c.name = "Velocity X";
        c.bodyColor = {0.90f, 0.12f, 0.15f};
        c.accentColor = {0.95f, 0.95f, 0.95f};
        c.maxSpeed = 52.0f;
        c.acceleration = 24.0f;
        c.brakeForce = 36.0f;
        c.steeringStrength = 2.3f;
        c.steeringAtHighSpeed = 0.42f;
        c.grip = 0.86f;
        c.driftFactor = 0.38f;
        c.nitroCapacity = 3.8f;
        c.nitroAcceleration = 38.0f;
        c.nitroMaxSpeedBonus = 20.0f;
        return c;
    }

    static VehicleConfig novaGT() {
        VehicleConfig c;
        c.id = "nova_gt";
        c.name = "Nova GT";
        c.bodyColor = {0.15f, 0.45f, 0.95f};
        c.accentColor = {0.2f, 0.9f, 1.0f};
        c.maxSpeed = 48.0f;
        c.acceleration = 28.0f;
        c.brakeForce = 40.0f;
        c.steeringStrength = 2.7f;
        c.steeringAtHighSpeed = 0.55f;
        c.grip = 0.92f;
        c.driftFactor = 0.28f;
        c.nitroCapacity = 3.2f;
        c.nitroAcceleration = 32.0f;
        c.nitroMaxSpeedBonus = 16.0f;
        return c;
    }

    static VehicleConfig cyberR() {
        VehicleConfig c;
        c.id = "cyber_r";
        c.name = "Cyber R";
        c.bodyColor = {0.15f, 0.85f, 0.45f};
        c.accentColor = {0.95f, 0.9f, 0.1f};
        c.maxSpeed = 56.0f;
        c.acceleration = 20.0f;
        c.brakeForce = 32.0f;
        c.steeringStrength = 2.1f;
        c.steeringAtHighSpeed = 0.38f;
        c.grip = 0.82f;
        c.driftFactor = 0.42f;
        c.nitroCapacity = 4.2f;
        c.nitroAcceleration = 42.0f;
        c.nitroMaxSpeedBonus = 22.0f;
        return c;
    }

    static VehicleConfig byId(const std::string& id) {
        if (id == "nova_gt") return novaGT();
        if (id == "cyber_r") return cyberR();
        return velocityX();
    }
};

} // namespace nexvora
