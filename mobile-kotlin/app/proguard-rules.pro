# Add project specific ProGuard rules here.
# By default, the flags in this file are appended to flags specified
# in C:\Users\...\AppData\Local\Android\Sdk/tools/proguard/proguard-android-optimize.txt

# Keep Kotlin reflect / metadata if needed
-keepclassmembers class * {
    @androidx.compose.runtime.Composable *;
}
