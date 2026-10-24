# Building SOL Launcher

SOL Launcher is a C++23 / Qt 6 Widgets application built with CMake. The build is the same as for the Prism Launcher
code base it is derived from; only the product identity (names, ids, URLs, API keys) differs.

## Getting the source

Clone with submodules. The repository pins two of them: `libraries/libnbtplusplus` (always needed) and `cmake/vcpkg`
(Microsoft's vcpkg, used by the CMake presets on Windows and macOS).

```
git clone --recurse-submodules https://github.com/solfain/prism-remake.git
```

If you already cloned without them: `git submodule update --init --recursive`.

**GitHub's "Download ZIP" does not contain submodules**, so a ZIP is not buildable as downloaded. If a ZIP is all you
have, fill the two folders in by hand with the commits this repository pins (GitHub shows the exact commit next to each
submodule folder; the values below are the ones pinned when this was written and change whenever a submodule is
updated):

```
git clone https://github.com/PrismLauncher/libnbtplusplus.git libraries/libnbtplusplus
git -C libraries/libnbtplusplus checkout 3538933614059f0f44388a2b16f3db25ce42285b

# only for the presets (Windows / macOS), not for a plain Linux build against system libraries:
git clone https://github.com/microsoft/vcpkg.git cmake/vcpkg
git -C cmake/vcpkg checkout 908da3a305a0a8028d9602ab241b433652b3df69
```

## Requirements

| What | Version | Notes |
| --- | --- | --- |
| Git | any recent | clone with `--recurse-submodules` (pulls `libnbtplusplus` and, for Windows/macOS builds, `vcpkg`) |
| CMake | 3.25+ | |
| Ninja | any recent | the presets use `Ninja Multi-Config` |
| C++23 compiler | MSVC 2022, GCC 13+, or Clang 17+ | **GCC 12 is too old**: its `std::expected` has no monadic operations (`and_then`, `transform_error`) and it hits an internal compiler error in `launcher/net/Request.cpp` |
| Qt | **6.8 or newer** | Core, Gui, Widgets, Network, Xml, Concurrent, OpenGL, Test, **NetworkAuth**; at run time also the Svg module and image-format plugins (all icons are SVG). Qt DBus is optional on Linux. Upstream CI currently builds with Qt 6.11. |
| JDK | 17 | builds the small helper jars (`NewLaunch.jar`, `JavaCheck.jar`). They are compiled with `-source 7`, which JDK 20+ no longer accepts. |
| Libraries | cmark, libarchive, toml++ >= 3.2, zlib, libqrencode, extra-cmake-modules | from vcpkg on Windows/macOS, from your distribution on Linux |

## Windows (MSVC)

1. Install **Visual Studio 2022** (or the Build Tools) with the *Desktop development with C++* workload, **CMake**,
   **Ninja**, **JDK 17** (set `JAVA_HOME`), and **Qt 6.8+ for MSVC 2022 64-bit**. In the Qt online installer, add
   *Qt Network Authorization* and *Qt Image Formats* under *Additional Libraries*.
2. Clone with submodules:
   ```
   git clone --recurse-submodules https://github.com/solfain/prism-remake.git
   cd prism-remake
   ```
3. From an *x64 Native Tools Command Prompt for VS 2022*:
   ```
   set CMAKE_PREFIX_PATH=C:\Qt\6.8.3\msvc2022_64
   cmake --preset windows_msvc
   cmake --build --preset windows_msvc --config Release
   cmake --install build --config Release --prefix install
   ```
   The `windows_msvc` preset (in `CMakePresets.json`) uses the vcpkg toolchain from `cmake/vcpkg`, which builds
   cmark, libarchive, toml++, zlib and libqrencode for you on the first configure. `cmake --install` also deploys the
   Qt DLLs and plugins next to `sollauncher.exe`, so the `install` folder is a complete, runnable installation.
4. Optional: build the installer with NSIS (`makensis build\program_info\win_install.nsi`).

The repository also contains a Prism Launcher installer (`PrismLauncher-Windows-MSVC-Setup-11.1.1.exe`) that was
uploaded before the source was added. It is not used by the build; consider removing it from the repository.

## Linux

Install a C++23 toolchain (GCC 13+ or Clang 17+), CMake, Ninja, JDK 17 and the Qt 6.8+ development packages including
`qtnetworkauth`, `qtsvg` and `qtimageformats`, plus development packages for cmark, libarchive, toml++, zlib,
libqrencode and extra-cmake-modules. Then:

```
git clone --recurse-submodules https://github.com/solfain/prism-remake.git
cd prism-remake
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build build
sudo cmake --install build
```

On distributions that package Qt in a non-default prefix, add `-DCMAKE_PREFIX_PATH=/path/to/qt`.

## macOS

Use the `macos` (or `macos_universal`) preset with Qt 6.8+, JDK 17 and Xcode. The macOS app icon bundle
(`program_info/SOLLauncher.icon`) is only compiled on Xcode 26+. The macOS build has not been tested for SOL Launcher.

## Configuration options that matter for SOL Launcher

All of these are CMake cache variables (`-DName=value`); the defaults live in the top-level `CMakeLists.txt`.

| Variable | Default | Purpose |
| --- | --- | --- |
| `Launcher_MSA_CLIENT_ID` | *(empty)* | Your Microsoft Entra application (client) ID. Without it "Add Microsoft account" opens a guided setup (see [MICROSOFT_SIGN_IN.md](MICROSOFT_SIGN_IN.md)). |
| `Launcher_CURSEFORGE_API_KEY` | *(empty)* | Your CurseForge API key. Without it CurseForge browsing and pack lookups are disabled. |
| `Launcher_IMGUR_CLIENT_ID` | *(empty)* | Your Imgur client ID. Without it the screenshot "Upload" action is hidden. |
| `Launcher_META_URL` | Prism Launcher project's metadata | Where Minecraft / loader version lists come from. |
| `Launcher_BUILD_ARTIFACT` | *(empty)* | Set together with `Launcher_UPDATER_GITHUB_REPO` to build the self-updater. |
| `Launcher_BUILD_PLATFORM` | `unknown` | Free-form label shown in the About dialog. |

The Microsoft client ID and the CurseForge key can also be entered at run time in **Settings > APIs**, which overrides
the build-time value. See [FORK_NOTES.md](FORK_NOTES.md) for why the keys are empty by default.

## Running without installing

After building, the executable is written to the top of the build folder: `build/sollauncher` with a single-config
generator (the Linux commands above), or `build/Release/sollauncher` (`sollauncher.exe` on Windows) with the
multi-config presets. On Windows run it from the install folder created by `cmake --install` rather than from `build`,
because only the install step copies the Qt DLLs and plugins next to it. Data goes to the `SOLLauncher` folder
described in the [README](../../README.md); pass `--dir <folder>` to use a scratch location while testing.
