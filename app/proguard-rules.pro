# NexVora Velocity ProGuard rules
# Keep native methods
-keepclasseswithmembernames class * {
    native <methods>;
}

-keep class com.nexvora.velocity.NativeBridge { *; }
-keep class com.nexvora.velocity.MainActivity { *; }
-keep class com.nexvora.velocity.GameGLSurfaceView { *; }
