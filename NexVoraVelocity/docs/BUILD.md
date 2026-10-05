# Build Documentation — NexVora Velocity Phase 1

## Prerequisites

- JDK 17
- Android SDK Platform 34
- Android NDK 26.1.10909125
- CMake 3.22.1 (or newer compatible)
- Git

On Linux / macOS the Gradle wrapper will download the correct Gradle distribution automatically.

## Local Command-Line Build

```bash
# From repository root
chmod +x gradlew
./gradlew :app:assembleDebug
```

Debug APK location:

```
app/build/outputs/apk/debug/app-debug.apk
```

### Useful Gradle tasks

```bash
./gradlew :app:assembleDebug      # Debug APK
./gradlew :app:assembleRelease    # Release APK (unsigned unless signing configured)
./gradlew :app:bundleRelease      # Release AAB
./gradlew :app:installDebug       # Install on device/emulator
./gradlew clean                   # Clean build outputs
./gradlew :app:externalNativeBuildDebug  # Native only
```

## Environment Variables (optional)

```bash
export ANDROID_HOME=/path/to/Android/Sdk
export ANDROID_NDK_HOME=$ANDROID_HOME/ndk/26.1.10909125
export JAVA_HOME=/path/to/jdk-17
```

A `local.properties` file is generated automatically by Android Studio or can be created manually:

```
sdk.dir=/path/to/Android/Sdk
```

This file is git-ignored.

## CI — GitHub Actions

File: `.github/workflows/android.yml`

Flow:

1. `actions/checkout@v4`
2. `actions/setup-java@v4` (Temurin 17 + Gradle cache)
3. `android-actions/setup-android@v3`
4. Install NDK 26.1.10909125 + CMake 3.22.1 via sdkmanager
5. `chmod +x gradlew`
6. `./gradlew :app:assembleDebug --stacktrace --no-daemon`
7. Upload `app-debug.apk` as artifact

## CI — Codemagic

File: `codemagic.yaml`

- Uses mac_mini_m1 instance
- Java 17
- Builds debug APK
- Optionally builds release APK / AAB (signing must be configured in Codemagic UI)
- Publishes APK/AAB artifacts

## Troubleshooting

**NDK not found**  
Ensure `ndkVersion = "26.1.10909125"` matches an installed NDK and that `ANDROID_NDK_HOME` points to it.

**CMake version**  
The project pins CMake 3.22.1. Install via SDK Manager if missing.

**OpenGL ES 3.0**  
The app requires a device/emulator with OpenGL ES 3.0 support (virtually all modern devices).

**JNI UnsatisfiedLinkError**  
Confirm the library name `nexvora_velocity` matches `System.loadLibrary` and the CMake target.

**Build fails on first run**  
Gradle downloads dependencies and the NDK toolchains; the first build can take several minutes.
