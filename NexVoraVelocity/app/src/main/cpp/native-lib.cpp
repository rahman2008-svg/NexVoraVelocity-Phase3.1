#include <jni.h>
#include <android/log.h>
#include "engine/Engine.h"
#include "core/Logger.h"

extern "C" {

JNIEXPORT void JNICALL
Java_com_nexvora_velocity_NativeBridge_onSurfaceCreated(JNIEnv*, jobject) {
    nexvora::Engine::instance().onSurfaceCreated();
}

JNIEXPORT void JNICALL
Java_com_nexvora_velocity_NativeBridge_onSurfaceChanged(JNIEnv*, jobject, jint width, jint height) {
    nexvora::Engine::instance().onSurfaceChanged(width, height);
}

JNIEXPORT void JNICALL
Java_com_nexvora_velocity_NativeBridge_onDrawFrame(JNIEnv*, jobject, jfloat deltaTime) {
    nexvora::Engine::instance().onDrawFrame(deltaTime);
}

JNIEXPORT void JNICALL
Java_com_nexvora_velocity_NativeBridge_onTouch(JNIEnv*, jobject, jint action, jint pointerId, jfloat x, jfloat y) {
    nexvora::Engine::instance().onTouch(action, pointerId, x, y);
}

JNIEXPORT void JNICALL
Java_com_nexvora_velocity_NativeBridge_onPause(JNIEnv*, jobject) {
    nexvora::Engine::instance().onPause();
}

JNIEXPORT void JNICALL
Java_com_nexvora_velocity_NativeBridge_onResume(JNIEnv*, jobject) {
    nexvora::Engine::instance().onResume();
}

JNIEXPORT void JNICALL
Java_com_nexvora_velocity_NativeBridge_onDestroy(JNIEnv*, jobject) {
    nexvora::Engine::instance().onDestroy();
}

JNIEXPORT void JNICALL
Java_com_nexvora_velocity_NativeBridge_setFilesDir(JNIEnv* env, jobject, jstring path) {
    const char* cpath = env->GetStringUTFChars(path, nullptr);
    if (cpath) {
        nexvora::Engine::instance().setFilesDir(cpath);
        env->ReleaseStringUTFChars(path, cpath);
    }
}

JNIEXPORT void JNICALL
Java_com_nexvora_velocity_NativeBridge_uiStartRace(JNIEnv*, jobject) {
    nexvora::Engine::instance().uiStartRace();
}

JNIEXPORT void JNICALL
Java_com_nexvora_velocity_NativeBridge_uiSelectCar(JNIEnv* env, jobject, jstring carId) {
    const char* id = env->GetStringUTFChars(carId, nullptr);
    if (id) {
        nexvora::Engine::instance().uiSelectCar(id);
        env->ReleaseStringUTFChars(carId, id);
    }
}

JNIEXPORT void JNICALL
Java_com_nexvora_velocity_NativeBridge_uiSetDifficulty(JNIEnv*, jobject, jint d) {
    nexvora::Engine::instance().uiSetDifficulty(d);
}

JNIEXPORT void JNICALL
Java_com_nexvora_velocity_NativeBridge_uiPause(JNIEnv*, jobject) {
    nexvora::Engine::instance().uiPause();
}

JNIEXPORT void JNICALL
Java_com_nexvora_velocity_NativeBridge_uiResume(JNIEnv*, jobject) {
    nexvora::Engine::instance().uiResume();
}

JNIEXPORT void JNICALL
Java_com_nexvora_velocity_NativeBridge_uiRestart(JNIEnv*, jobject) {
    nexvora::Engine::instance().uiRestart();
}

JNIEXPORT void JNICALL
Java_com_nexvora_velocity_NativeBridge_uiMainMenu(JNIEnv*, jobject) {
    nexvora::Engine::instance().uiMainMenu();
}

JNIEXPORT void JNICALL
Java_com_nexvora_velocity_NativeBridge_uiOpenCarSelect(JNIEnv*, jobject) {
    nexvora::Engine::instance().uiOpenCarSelect();
}

JNIEXPORT void JNICALL
Java_com_nexvora_velocity_NativeBridge_uiOpenSettings(JNIEnv*, jobject) {
    nexvora::Engine::instance().uiOpenSettings();
}

JNIEXPORT void JNICALL
Java_com_nexvora_velocity_NativeBridge_uiSetGraphics(JNIEnv*, jobject, jint q) {
    nexvora::Engine::instance().uiSetGraphics(q);
}

JNIEXPORT void JNICALL
Java_com_nexvora_velocity_NativeBridge_uiSetSoundVolume(JNIEnv*, jobject, jfloat v) {
    nexvora::Engine::instance().uiSetSoundVolume(v);
}

JNIEXPORT void JNICALL
Java_com_nexvora_velocity_NativeBridge_uiSetMusicVolume(JNIEnv*, jobject, jfloat v) {
    nexvora::Engine::instance().uiSetMusicVolume(v);
}

JNIEXPORT void JNICALL
Java_com_nexvora_velocity_NativeBridge_uiSetSensitivity(JNIEnv*, jobject, jfloat s) {
    nexvora::Engine::instance().uiSetSensitivity(s);
}

JNIEXPORT jint JNICALL
Java_com_nexvora_velocity_NativeBridge_uiGetState(JNIEnv*, jobject) {
    return nexvora::Engine::instance().uiGetState();
}

JNIEXPORT void JNICALL
Java_com_nexvora_velocity_NativeBridge_uiGetHud(JNIEnv* env, jobject, jfloatArray out) {
    // out[12]: speed,lap,totalLaps,pos,raceTime,nitro,countdown,state,bestLap,finalTime,finalPos,difficulty
    jfloat buf[12] = {0};
    nexvora::Engine::instance().uiGetHud(buf);
    jsize len = env->GetArrayLength(out);
    jsize n = len < 12 ? len : 12;
    env->SetFloatArrayRegion(out, 0, n, buf);
}

} // extern "C"
