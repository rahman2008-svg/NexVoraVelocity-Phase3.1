package com.nexvora.velocity

/**
 * JNI bridge to the native C++ game engine.
 */
object NativeBridge {

    init {
        System.loadLibrary("nexvora_velocity")
    }

    external fun onSurfaceCreated()
    external fun onSurfaceChanged(width: Int, height: Int)
    external fun onDrawFrame(deltaTime: Float)
    external fun onTouch(action: Int, pointerId: Int, x: Float, y: Float)
    external fun onPause()
    external fun onResume()
    external fun onDestroy()

    external fun setFilesDir(path: String)

    // UI / game control
    external fun uiStartRace()
    external fun uiSelectCar(carId: String)
    external fun uiSetDifficulty(difficulty: Int)
    external fun uiPause()
    external fun uiResume()
    external fun uiRestart()
    external fun uiMainMenu()
    external fun uiOpenCarSelect()
    external fun uiOpenSettings()
    external fun uiSetGraphics(quality: Int)
    external fun uiSetSoundVolume(v: Float)
    external fun uiSetMusicVolume(v: Float)
    external fun uiSetSensitivity(s: Float)
    external fun uiGetState(): Int
    external fun uiGetHud(out: FloatArray)
}
