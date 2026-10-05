# NexVora Velocity — Phase 3 Complete Arcade Racing Game

**NexVora Velocity** is a fully playable offline 3D arcade racing game for Android, built on a custom lightweight engine (Kotlin + C++ + OpenGL ES 3.0 + NDK).

No Unity, Unreal, Godot, Defold, Flutter, or other external game engines.

---

## Play Flow

```
Main Menu → Select Car → Start Race → Countdown (3-2-1-GO)
→ Drive (steer / accel / brake / nitro) → Pass checkpoints
→ Race 3 AI opponents → Complete 3 laps → Results → Restart / Menu
```

---

## Features

### Vehicles
- **Velocity X** — balanced speed
- **Nova GT** — best handling & acceleration
- **Cyber R** — highest top speed & nitro

Arcade controller: accelerate, brake, reverse, steering, grip, drift, nitro, collision response, respawn.

### Track
Futuristic oval circuit with road segments, barriers, checkpoints, start/finish gate, buildings, trees, street lights, fog.

### Race Systems
- 3-lap races (configurable)
- Checkpoint validation
- Position (1st–4th) via lap + track progress
- AI opponents (Easy / Normal / Hard)
- Nitro boost with meter
- Particle effects (exhaust, nitro, smoke, sparks)
- Chase camera with speed-based distance & shake
- Local save (car, difficulty, best times, settings)

### UI
- Main menu, car selection, settings
- In-race HUD (speed, lap, position, time, nitro, countdown)
- Pause menu, results screen
- Touch controls: left = steer, right = accel/brake/nitro

---

## Build

```bash
git clone <repo-url> NexVoraVelocity
cd NexVoraVelocity
chmod +x gradlew
./gradlew :app:assembleDebug
./gradlew :app:installDebug
```

APK: `app/build/outputs/apk/debug/app-debug.apk`

---

## Controls (Touch)

| Zone | Action |
|------|--------|
| Left half | Steering (slide left/right) |
| Right mid | Accelerate |
| Right bottom | Brake / Reverse |
| Right top | Nitro |

---

## Tech

| Component | Version |
|-----------|---------|
| Gradle | 8.7 |
| AGP | 8.5.2 |
| Kotlin | 1.9.24 |
| compileSdk | 34 |
| NDK | 26.1.10909125 |
| CMake | 3.22.1 |
| OpenGL ES | 3.0 |
| minSdk | 24 |

---

## CI

- **GitHub Actions**: builds debug APK automatically
- **Codemagic**: debug + release-ready

---

## Architecture

```
Kotlin UI (menus, HUD) ←→ JNI ←→ C++ Engine
                              ├── RaceManager
                              ├── VehicleController
                              ├── Track / Checkpoints / Waypoints
                              ├── AIController
                              ├── ParticleSystem
                              ├── LocalSave
                              └── Renderer / Scene / Camera / Input
```

---

Copyright © NexVora. Fictional cars and branding only.
