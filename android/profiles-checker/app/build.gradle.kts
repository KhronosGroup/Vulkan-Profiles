plugins {
    id("com.android.application")
}

android {
    namespace = "org.khronos.vulkanprofiles.checker"
    compileSdk = 35
    ndkVersion = "27.0.12077973"

    defaultConfig {
        applicationId = "org.khronos.vulkanprofiles.checker"
        minSdk = 26
        // targetSdk 35+ enforces edge-to-edge and the system bar insets are only
        // exposed through Java WindowInsets APIs, which a code-less NativeActivity
        // cannot use. Targeting 34 keeps the window laid out between the bars.
        targetSdk = 34
        versionCode = 1
        versionName = "1.0"

        ndk {
            // arm64 for real devices, x86_64 for the emulator.
            abiFilters += listOf("arm64-v8a", "x86_64")
        }

        externalNativeBuild {
            cmake {
                arguments += listOf(
                    "-DANDROID_STL=c++_static",
                    "-DANDROID_CPP_FEATURES=exceptions"
                )
            }
        }
    }

    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            // Debug-signed so the release APK can still be side-loaded for testing.
            signingConfig = signingConfigs.getByName("debug")
        }
    }
}
