package com.nexvora.velocity

import android.annotation.SuppressLint
import android.content.pm.ActivityInfo
import android.os.Build
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.view.MotionEvent
import android.view.View
import android.view.WindowInsets
import android.view.WindowInsetsController
import android.view.WindowManager
import android.widget.Button
import android.widget.LinearLayout
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {

    private lateinit var glSurfaceView: GameGLSurfaceView
    private var isSurfaceReady = false
    private val hudHandler = Handler(Looper.getMainLooper())
    private val hudData = FloatArray(12)

    private lateinit var panelMain: LinearLayout
    private lateinit var panelCars: LinearLayout
    private lateinit var panelSettings: LinearLayout
    private lateinit var panelHud: LinearLayout
    private lateinit var panelPause: LinearLayout
    private lateinit var panelResults: LinearLayout

    private lateinit var txtSpeed: TextView
    private lateinit var txtLap: TextView
    private lateinit var txtPosition: TextView
    private lateinit var txtTime: TextView
    private lateinit var txtNitro: TextView
    private lateinit var txtCountdown: TextView
    private lateinit var txtSelectedCar: TextView
    private lateinit var txtResultPos: TextView
    private lateinit var txtResultTime: TextView
    private lateinit var txtResultLap: TextView
    private lateinit var txtControlsHint: TextView

    private var selectedCarId = "velocity_x"
    private var lastUiState = -1

    private val hudRunnable = object : Runnable {
        override fun run() {
            updateHudFromNative()
            hudHandler.postDelayed(this, 50)
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        requestedOrientation = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE
        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)

        setContentView(R.layout.activity_main)
        setupImmersiveMode()

        NativeBridge.setFilesDir(filesDir.absolutePath)

        bindViews()
        setupButtons()
        setupGLSurfaceView()

        showPanel(panelMain)
        hudHandler.post(hudRunnable)
    }

    private fun bindViews() {
        panelMain = findViewById(R.id.panel_main_menu)
        panelCars = findViewById(R.id.panel_cars)
        panelSettings = findViewById(R.id.panel_settings)
        panelHud = findViewById(R.id.panel_hud)
        panelPause = findViewById(R.id.panel_pause)
        panelResults = findViewById(R.id.panel_results)

        txtSpeed = findViewById(R.id.txt_speed)
        txtLap = findViewById(R.id.txt_lap)
        txtPosition = findViewById(R.id.txt_position)
        txtTime = findViewById(R.id.txt_time)
        txtNitro = findViewById(R.id.txt_nitro)
        txtCountdown = findViewById(R.id.txt_countdown)
        txtSelectedCar = findViewById(R.id.txt_selected_car)
        txtResultPos = findViewById(R.id.txt_result_pos)
        txtResultTime = findViewById(R.id.txt_result_time)
        txtResultLap = findViewById(R.id.txt_result_lap)
        txtControlsHint = findViewById(R.id.txt_controls_hint)
    }

    private fun setupButtons() {
        findViewById<Button>(R.id.btn_race).setOnClickListener {
            NativeBridge.uiSelectCar(selectedCarId)
            NativeBridge.uiStartRace()
        }
        findViewById<Button>(R.id.btn_cars).setOnClickListener {
            NativeBridge.uiOpenCarSelect()
            showPanel(panelCars)
        }
        findViewById<Button>(R.id.btn_settings).setOnClickListener {
            NativeBridge.uiOpenSettings()
            showPanel(panelSettings)
        }
        findViewById<Button>(R.id.btn_exit).setOnClickListener { finish() }

        findViewById<Button>(R.id.btn_car_vx).setOnClickListener {
            selectedCarId = "velocity_x"
            NativeBridge.uiSelectCar(selectedCarId)
            txtSelectedCar.text = "Selected: Velocity X"
        }
        findViewById<Button>(R.id.btn_car_nova).setOnClickListener {
            selectedCarId = "nova_gt"
            NativeBridge.uiSelectCar(selectedCarId)
            txtSelectedCar.text = "Selected: Nova GT"
        }
        findViewById<Button>(R.id.btn_car_cyber).setOnClickListener {
            selectedCarId = "cyber_r"
            NativeBridge.uiSelectCar(selectedCarId)
            txtSelectedCar.text = "Selected: Cyber R"
        }
        findViewById<Button>(R.id.btn_cars_back).setOnClickListener {
            NativeBridge.uiMainMenu()
            showPanel(panelMain)
        }
        findViewById<Button>(R.id.btn_cars_race).setOnClickListener {
            NativeBridge.uiSelectCar(selectedCarId)
            NativeBridge.uiStartRace()
        }

        findViewById<Button>(R.id.btn_diff_easy).setOnClickListener { NativeBridge.uiSetDifficulty(0) }
        findViewById<Button>(R.id.btn_diff_normal).setOnClickListener { NativeBridge.uiSetDifficulty(1) }
        findViewById<Button>(R.id.btn_diff_hard).setOnClickListener { NativeBridge.uiSetDifficulty(2) }
        findViewById<Button>(R.id.btn_gfx_low).setOnClickListener { NativeBridge.uiSetGraphics(0) }
        findViewById<Button>(R.id.btn_gfx_med).setOnClickListener { NativeBridge.uiSetGraphics(1) }
        findViewById<Button>(R.id.btn_gfx_high).setOnClickListener { NativeBridge.uiSetGraphics(2) }
        findViewById<Button>(R.id.btn_settings_back).setOnClickListener {
            NativeBridge.uiMainMenu()
            showPanel(panelMain)
        }

        findViewById<Button>(R.id.btn_pause).setOnClickListener { NativeBridge.uiPause() }
        findViewById<Button>(R.id.btn_resume).setOnClickListener { NativeBridge.uiResume() }
        findViewById<Button>(R.id.btn_restart).setOnClickListener { NativeBridge.uiRestart() }
        findViewById<Button>(R.id.btn_pause_menu).setOnClickListener {
            NativeBridge.uiMainMenu()
            showPanel(panelMain)
        }

        findViewById<Button>(R.id.btn_result_again).setOnClickListener { NativeBridge.uiRestart() }
        findViewById<Button>(R.id.btn_result_cars).setOnClickListener {
            NativeBridge.uiOpenCarSelect()
            showPanel(panelCars)
        }
        findViewById<Button>(R.id.btn_result_menu).setOnClickListener {
            NativeBridge.uiMainMenu()
            showPanel(panelMain)
        }
    }

    private fun setupGLSurfaceView() {
        val root = findViewById<android.widget.FrameLayout>(R.id.root_layout)
        glSurfaceView = GameGLSurfaceView(this)
        // Insert GL behind panels
        root.addView(glSurfaceView, 0)
        isSurfaceReady = true
    }

    private fun showPanel(panel: LinearLayout) {
        panelMain.visibility = View.GONE
        panelCars.visibility = View.GONE
        panelSettings.visibility = View.GONE
        panelHud.visibility = View.GONE
        panelPause.visibility = View.GONE
        panelResults.visibility = View.GONE
        txtControlsHint.visibility = View.GONE
        panel.visibility = View.VISIBLE
    }

    private fun formatTime(seconds: Float): String {
        val totalCents = (seconds * 100).toInt().coerceAtLeast(0)
        val m = totalCents / 6000
        val s = (totalCents / 100) % 60
        val c = totalCents % 100
        return "%d:%02d.%02d".format(m, s, c)
    }

    private fun positionSuffix(pos: Int): String {
        return when (pos) {
            1 -> "1st"
            2 -> "2nd"
            3 -> "3rd"
            else -> "${pos}th"
        }
    }

    private fun nitroBar(n: Float): String {
        val filled = (n * 8).toInt().coerceIn(0, 8)
        return "NITRO " + "█".repeat(filled) + "░".repeat(8 - filled)
    }

    private fun updateHudFromNative() {
        try {
            NativeBridge.uiGetHud(hudData)
        } catch (e: Exception) {
            return
        }
        val state = hudData[7].toInt()
        if (state != lastUiState) {
            lastUiState = state
            when (state) {
                0 -> showPanel(panelMain)           // MainMenu
                1 -> showPanel(panelCars)           // CarSelection
                2 -> showPanel(panelSettings)       // Settings
                3, 4 -> {                           // Countdown / Racing
                    showPanel(panelHud)
                    txtControlsHint.visibility = View.VISIBLE
                }
                5 -> {                              // Paused
                    panelHud.visibility = View.VISIBLE
                    panelPause.visibility = View.VISIBLE
                }
                6, 7 -> showPanel(panelResults)     // Finished / Results
            }
        }

        if (state == 3 || state == 4 || state == 5) {
            txtSpeed.text = "%.0f km/h".format(hudData[0])
            txtLap.text = "Lap %d/%d".format(hudData[1].toInt().coerceAtLeast(1), hudData[2].toInt())
            txtPosition.text = positionSuffix(hudData[3].toInt().coerceIn(1, 4))
            txtTime.text = formatTime(hudData[4])
            txtNitro.text = nitroBar(hudData[5])
            val cd = hudData[6].toInt()
            when {
                cd > 0 -> {
                    txtCountdown.visibility = View.VISIBLE
                    txtCountdown.text = cd.toString()
                }
                cd == 0 -> {
                    txtCountdown.visibility = View.VISIBLE
                    txtCountdown.text = "GO!"
                }
                else -> {
                    txtCountdown.visibility = View.GONE
                    txtCountdown.text = ""
                }
            }
        }

        if (state == 7 || state == 6) {
            val finalPos = hudData[10].toInt().let { if (it > 0) it else hudData[3].toInt() }.coerceIn(1, 4)
            val finalTime = if (hudData[9] > 0f) hudData[9] else hudData[4]
            val bestLap = hudData[8]
            txtResultPos.text = "Position: ${positionSuffix(finalPos)}"
            txtResultTime.text = "Time: ${formatTime(finalTime)}"
            txtResultLap.text = "Best Lap: ${if (bestLap > 0f) formatTime(bestLap) else "--"}"
        }
    }

    private fun setupImmersiveMode() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            window.setDecorFitsSystemWindows(false)
            window.insetsController?.let { controller ->
                controller.hide(WindowInsets.Type.statusBars() or WindowInsets.Type.navigationBars())
                controller.systemBarsBehavior =
                    WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE
            }
        } else {
            @Suppress("DEPRECATION")
            window.decorView.systemUiVisibility = (
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                    or View.SYSTEM_UI_FLAG_FULLSCREEN
                    or View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                    or View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                    or View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                    or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                )
        }
    }

    override fun onResume() {
        super.onResume()
        if (isSurfaceReady) {
            glSurfaceView.onResume()
            NativeBridge.onResume()
        }
        setupImmersiveMode()
    }

    override fun onPause() {
        if (isSurfaceReady) {
            NativeBridge.onPause()
            glSurfaceView.onPause()
        }
        super.onPause()
    }

    override fun onDestroy() {
        hudHandler.removeCallbacks(hudRunnable)
        if (isSurfaceReady) {
            NativeBridge.onDestroy()
        }
        super.onDestroy()
    }

    override fun onWindowFocusChanged(hasFocus: Boolean) {
        super.onWindowFocusChanged(hasFocus)
        if (hasFocus) setupImmersiveMode()
    }

    @SuppressLint("ClickableViewAccessibility")
    override fun onTouchEvent(event: MotionEvent?): Boolean {
        event ?: return super.onTouchEvent(event)
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
            NativeBridge.onTouch(action, pointerId, x, y)
        }
        return true
    }
}
