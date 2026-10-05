package com.nexvora.velocity

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.AttributeSet
import android.view.MotionEvent
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class GameGLSurfaceView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null
) : GLSurfaceView(context, attrs) {

    private val renderer: GameRenderer

    init {
        setEGLContextClientVersion(3)
        setEGLConfigChooser(8, 8, 8, 8, 24, 8)
        renderer = GameRenderer()
        setRenderer(renderer)
        renderMode = RENDERMODE_CONTINUOUSLY
        preserveEGLContextOnPause = true
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        val action = when (event.actionMasked) {
            MotionEvent.ACTION_DOWN -> 0
            MotionEvent.ACTION_MOVE -> 1
            MotionEvent.ACTION_UP, MotionEvent.ACTION_CANCEL -> 2
            MotionEvent.ACTION_POINTER_DOWN -> 3
            MotionEvent.ACTION_POINTER_UP -> 4
            else -> -1
        }
        if (action >= 0) {
            val pointerIndex = event.actionIndex
            val pointerId = event.getPointerId(pointerIndex)
            val x = event.getX(pointerIndex)
            val y = event.getY(pointerIndex)
            queueEvent {
                NativeBridge.onTouch(action, pointerId, x, y)
            }
        }
        return true
    }

    private class GameRenderer : Renderer {
        private var lastFrameTimeNs = 0L

        override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
            NativeBridge.onSurfaceCreated()
            lastFrameTimeNs = System.nanoTime()
        }

        override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
            NativeBridge.onSurfaceChanged(width, height)
        }

        override fun onDrawFrame(gl: GL10?) {
            val now = System.nanoTime()
            val deltaTime = if (lastFrameTimeNs == 0L) {
                0.016f
            } else {
                ((now - lastFrameTimeNs) / 1_000_000_000.0).toFloat().coerceIn(0.0f, 0.1f)
            }
            lastFrameTimeNs = now
            NativeBridge.onDrawFrame(deltaTime)
        }
    }
}
