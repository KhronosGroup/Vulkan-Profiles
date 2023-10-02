# Build Instructions

This document contains the instructions for building this repository.

## System Requirements

### Windows System Requirements

- Microsoft Visual Studio 2019 or higher
- [CMake 3.17.2](https://github.com/Kitware/CMake/releases/download/v3.17.2/cmake-3.17.2-win64-x64.zip) is recommended.
  - Tell the installer to "Add CMake to the system `PATH`" environment variable.
- Python 3.7.2 or later (from https://www.python.org/downloads).  Notes:
  - Select to install the optional sub-package to add Python to the system `PATH` environment variable.
  - Need python3.3 or later to get the Windows `py.exe` launcher that is used to get `python3` rather than `python2` if both are installed on Windows
  - Install `jsonschema` package (`pip3 install jsonschema`)
- Git (from http://git-scm.com/download/win).
  - Tell the installer to allow it to be used for "Developer Prompt" as well as "Git Bash".
  - Tell the installer to treat line endings "as is" (i.e. both DOS and Unix-style line endings).

Optional software packages:

- *Cygwin* (from https://www.cygwin.com/).
  - *Cygwin* provides some Linux-like tools, which are valuable for obtaining the source code, and running CMake.
    Especially valuable are the *BASH* shell and git packages.
  - If you do not wish to use *Cygwin*, there are other shells and environments that can be used.
    You can also use a Git package that does not come from *Cygwin*.

### Ubuntu System Requirements

[CMake 3.17.2](https://github.com/Kitware/CMake/releases/download/v3.17.2/cmake-3.17.2-Linux-x86_64.tar.gz) is recommended.

```
# Dependencies from included submodule components
sudo apt-get install git build-essential bison libx11-xcb-dev libxkbcommon-dev libwayland-dev libxrandr-dev libxcb-randr0-dev

# Additional dependencies for this repo:
sudo apt-get install wget autotools-dev libxcb-keysyms1 libxcb-keysyms1-dev libxcb-ewmh-dev

# If performing 32-bit builds, you will also need:
sudo apt-get install libc6-dev-i386 g++-multilib

# Install jsonschema python package
sudo apt-get install python3-pip
pip3 install jsonschema
```

### Fedora Core System Requirements

Fedora Core 28 and 29 were tested with this repo.

[CMake 3.17.2](https://github.com/Kitware/CMake/releases/download/v3.17.2/cmake-3.17.2-Linux-x86_64.tar.gz) is recommended.

Additional package dependencies include:

```
# Dependencies from included submodule components
sudo dnf install git @development-tools glm-devel \
                 libpng-devel wayland-devel libpciaccess-devel \
                 libX11-devel libXpresent libxcb xcb-util libxcb-devel libXrandr-devel \
                 xcb-util-keysyms-devel xcb-util-wm-devel

# Install jsonschema python package
sudo dnf install python3-pip
pip3 install jsonschema
```

Optional software packages:

### macOS System Requirements

MacOS 10.15 have been tested with this repo.

- [CMake 3.17.2](https://github.com/Kitware/CMake/releases/download/v3.17.2/cmake-3.17.2-Darwin-x86_64.tar.gz) is recommended.

Setup Homebrew and components

- Ensure Homebrew is at the beginning of your PATH:
```
export PATH=/usr/local/bin:$PATH
```

- Add packages with the following (may need refinement)
```
brew install python python3 git
```

- Install `jsonschema` python package
```
pip3 install jsonschema
```

### Android Additional System Requirements
Install the required tools for Linux and Windows covered above, then add the
following.

#### Install the Android SDK and NDK

- With Android Studio
  - Install [Android Studio 2.3](https://developer.android.com/studio/index.html)
    or later.
  - From the "Welcome to Android Studio" splash screen, add the following
    components using Configure > SDK Manager:
    - SDK Platforms > Android 8.0.0 and newer
    - SDK Tools > Android SDK Build-Tools
    - SDK Tools > Android SDK Platform-Tools
    - SDK Tools > Android SDK Tools
    - SDK Tools > NDK
- [sdkmanager](https://developer.android.com/studio/command-line/sdkmanager) CLI tool

#### Set SDK/NDK Build Environment Variables

Some common locations for the `ANDROID_SDK_HOME` and `ANDROID_NDK_HOME` are:

On Linux:

```bash
    export ANDROID_SDK_HOME=$HOME/Android/sdk
    export ANDROID_NDK_HOME=$HOME/Android/sdk/ndk-bundle
```

On Windows:

```batch
    set ANDROID_SDK_HOME=%LOCALAPPDATA%\Android\sdk
    set ANDROID_NDK_HOME=%LOCALAPPDATA%\Android\sdk\ndk-bundle
```

On OSX:

```bash
    export ANDROID_SDK_HOME=$HOME/Library/Android/sdk
    export ANDROID_NDK_HOME=$HOME/Library/Android/sdk/ndk-bundle
```

## Getting Started Build Instructions

### 64-bit Windows Build 
```
git clone git@github.com:KhronosGroup/Vulkan-Profiles.git
cd Vulkan-Profiles
cmake -S . -B build/ -D CMAKE_BUILD_TYPE=Debug -D BUILD_TESTS=ON -D UPDATE_DEPS=ON -A x64
cmake --build ./build/ --config Debug
```

### Windows Unit Tests

```
cd build/
ctest -C Debug --output-on-failure --parallel 16
```

### Linux and macOS Build
```
git clone git@github.com:KhronosGroup/Vulkan-Profiles.git
cd Vulkan-Profiles
cmake -S . -B build/ -D CMAKE_BUILD_TYPE=Debug -D UPDATE_DEPS=ON
cmake --build ./build/
```

### Linux and macOS Unit Tests

```
cd build/
ctest --parallel 8 --output-on-failure
```

### Android Build
Use the following to ensure the Android build works.

#### Android Build using CMake For Windows, Linux, and OSX

```bash
cmake -Bandroid-build \
  -DCMAKE_TOOLCHAIN_FILE=<ndk-path>/build/cmake/android.toolchain.cmake \
  -DANDROID_PLATFORM=28 \
  -DANDROID_BUILD_TOOLS=30.0.3 \
  -DANDROID_ABI=arm64-v8a

cd android-build
cmake --build . --parallel
```

#### Android Unit Tests
The Android test packages are created in the following directory:
    Vulkan-Profiles/android-build/apk/out

```
adb install -r apk/out/TestPackageName.apk
adb shell am start -a android.intent.MAIN -c android-intent.category.LAUNCH -n com.example.TestPackageName/android.app.NativeActivity --es args --gtest_catch_exceptions=0
# Test output is written to logcat
adb logcat > tempfile
grep VulkanProfilesLayerTests: tempfile
```
