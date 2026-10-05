cmake_minimum_required(VERSION 3.22.1)

project(nexvora_velocity)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ENGINE_SOURCES
    native-lib.cpp
    core/Logger.cpp
    core/Time.cpp
    core/Math.cpp
    engine/Game.cpp
    engine/Engine.cpp
    render/Renderer.cpp
    render/Shader.cpp
    render/ShaderManager.cpp
    render/Texture.cpp
    render/TextureManager.cpp
    render/Mesh.cpp
    scene/Scene.cpp
    scene/Camera.cpp
    scene/Transform.cpp
    scene/GameObject.cpp
    input/InputManager.cpp
    resources/ResourceManager.cpp
    audio/AudioManager.cpp
    debug/DebugOverlay.cpp
    gameplay/VehicleController.cpp
    gameplay/Track.cpp
    gameplay/RaceManager.cpp
    gameplay/AIController.cpp
    gameplay/ParticleSystem.cpp
    gameplay/LocalSave.cpp
)

add_library(nexvora_velocity SHARED ${ENGINE_SOURCES})

target_include_directories(nexvora_velocity PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/render/materials
    ${CMAKE_CURRENT_SOURCE_DIR}/render/shaders
    ${CMAKE_CURRENT_SOURCE_DIR}/render/textures
    ${CMAKE_CURRENT_SOURCE_DIR}/render/buffers
    ${CMAKE_CURRENT_SOURCE_DIR}/scene
    ${CMAKE_CURRENT_SOURCE_DIR}/input
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/audio
    ${CMAKE_CURRENT_SOURCE_DIR}/debug
    ${CMAKE_CURRENT_SOURCE_DIR}/gameplay
)

find_library(log-lib log)
find_library(android-lib android)
find_library(EGL-lib EGL)
find_library(GLESv3-lib GLESv3)

target_link_libraries(nexvora_velocity
    ${log-lib}
    ${android-lib}
    ${EGL-lib}
    ${GLESv3-lib}
)

target_compile_options(nexvora_velocity PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
    -fvisibility=hidden
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(nexvora_velocity PRIVATE -O3 -DNDEBUG)
else()
    target_compile_options(nexvora_velocity PRIVATE -O0 -g)
endif()
