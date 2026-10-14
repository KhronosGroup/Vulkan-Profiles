plugins {
    id("com.android.application")
}

// -----------------------------------------------------------------------------
// Versioning (single source of truth; propagated to the native code via CMake)
//
//   versionName : semantic version shown to users and in the UI/report.
//   versionCode : monotonically increasing integer Android uses to decide
//                 whether an APK is an upgrade. Derived from the version so the
//                 two can never drift: MAJOR * 10000 + MINOR * 100 + PATCH.
//                 (1.0.0 -> 10000, 1.2.3 -> 10203, 2.0.0 -> 20000)
//
// Bump checkerVersion here for every release and tag the repo with
// profiles-checker-v<version>.
// -----------------------------------------------------------------------------
val checkerVersion = "1.0.0"
val checkerVersionCode: Int = checkerVersion.split(".").let { (major, minor, patch) ->
    major.toInt() * 10000 + minor.toInt() * 100 + patch.toInt()
}

// ABIs packaged into the APK. Default: universal. Override with
// -PvpCheckerAbis=armeabi-v7a (comma separated) for single-ABI test builds.
val allAbis = listOf("arm64-v8a", "armeabi-v7a", "x86_64", "x86")
val checkerAbis: List<String> = (findProperty("vpCheckerAbis") as String?)
    ?.split(",")?.map { it.trim() }?.filter { it.isNotEmpty() } ?: allAbis
val isUniversalAbiBuild = checkerAbis.toSet() == allAbis.toSet()

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
        versionCode = checkerVersionCode
        versionName = checkerVersion

        ndk {
            // All ABIs Android still ships: 64-bit and 32-bit Arm for devices,
            // x86_64 / x86 for emulators and Chromebooks. One universal APK.
            // Override for testing a single ABI, e.g. to force the 32-bit library
            // onto a 64-bit device:  ./gradlew assembleDebug -PvpCheckerAbis=armeabi-v7a
            abiFilters += checkerAbis
        }

        externalNativeBuild {
            cmake {
                arguments += listOf(
                    "-DANDROID_STL=c++_static",
                    "-DANDROID_CPP_FEATURES=exceptions",
                    "-DVP_CHECKER_VERSION_NAME=$checkerVersion",
                    "-DVP_CHECKER_VERSION_CODE=$checkerVersionCode"
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

    // The app is distributed as a *debug* build only: the generated profiles
    // library is the debug variant (it is what reports unmet requirements), and
    // a debug-signed APK can be side-loaded on any device without a release key.
    // No `release` build type is configured on purpose.

    // vulkan-profiles-checker-<version>-<buildType>[-<abi>].apk instead of app-<buildType>.apk
    applicationVariants.all {
        outputs.all {
            val abiSuffix = if (isUniversalAbiBuild) "" else "-" + checkerAbis.joinToString("+")
            (this as com.android.build.gradle.internal.api.BaseVariantOutputImpl).outputFileName =
                "vulkan-profiles-checker-${versionName}-${buildType.name}${abiSuffix}.apk"
        }
    }
}
