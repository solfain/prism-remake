# Changes compared to the Prism Launcher baseline

The first commit on this branch (`3012098`, *Import Prism Launcher develop as the SOL Launcher baseline*) is an
unmodified snapshot of Prism Launcher `develop`. Everything after it is SOL Launcher, in five commits. This page is
generated from those commits; for the full diff run `git diff 3012098..HEAD`.

| # | Commit | What it is |
| --- | --- | --- |
| 1 | Rebrand to SOL Launcher: identity, API keys, services and branding strings | Product identity: names, ids, data folder, version, API keys and service URLs, branding strings, About / license text |
| 2 | Add the SOL iceberg logo and application icons | The iceberg logo and every application icon format, plus the generator script |
| 3 | Add the SOL Light and SOL Dark themes | The SOL Light / SOL Dark themes (palette, stylesheet, `SolTheme`) and their unit tests |
| 4 | Restructure the main window: sidebar navigation, page header and instance cards | Main window: sidebar navigation, page header, instance cards, group headers, empty state, instance panel styling |
| 5 | Add SOL Launcher documentation | Documentation (README, `docs/sol/*`, screenshots) |

## 1. Rebrand to SOL Launcher: identity, API keys, services and branding strings

| Change | File | |
| --- | --- | --- |
| modified | `CMakeLists.txt` | |
| modified | `COPYING.md` | |
| modified | `cmake/MacOSXBundleInfo.plist.in` | |
| modified | `launcher/Application.cpp` | |
| modified | `launcher/main.cpp` | |
| modified | `launcher/filelink/FileLink.cpp` | |
| modified | `launcher/resources/documents/credits.html` | |
| modified | `launcher/ui/dialogs/AboutDialog.cpp` | |
| modified | `launcher/ui/pages/global/LauncherPage.ui` | |
| modified | `launcher/ui/pages/instance/ScreenshotsPage.cpp` | |
| modified | `launcher/ui/pages/modplatform/ImportPage.ui` | |
| modified | `launcher/ui/pages/modplatform/flame/FlamePage.ui` | |
| modified | `launcher/ui/setupwizard/JavaWizardPage.cpp` | |
| modified | `program_info/CMakeLists.txt` | |
| modified | `program_info/README.md` | |
| modified | `program_info/genicons.sh` | |
| modified | `program_info/win_install.nsi.in` | |
| renamed | `program_info/PrismLauncher.icon/Assets/block.svg -> program_info/SOLLauncher.icon/Assets/block.svg` | |
| renamed | `program_info/PrismLauncher.icon/Assets/rainbow.svg -> program_info/SOLLauncher.icon/Assets/rainbow.svg` | |
| renamed | `program_info/PrismLauncher.icon/icon.json -> program_info/SOLLauncher.icon/icon.json` | |
| renamed | `program_info/org.prismlauncher.PrismLauncher.desktop.in -> program_info/io.github.solfain.SOLLauncher.desktop.in` | |
| renamed | `program_info/org.prismlauncher.PrismLauncher.mime.xml -> program_info/io.github.solfain.SOLLauncher.mime.xml` | |
| renamed | `program_info/org.prismlauncher.PrismLauncher.svg -> program_info/io.github.solfain.SOLLauncher.svg` | |
| renamed | `program_info/org.prismlauncher.PrismLauncher_256.png -> program_info/io.github.solfain.SOLLauncher_256.png` | |
| renamed | `program_info/prismlauncher.6.scd.in -> program_info/sollauncher.6.scd.in` | |
| renamed | `program_info/prismlauncher.icns -> program_info/sollauncher.icns` | |
| renamed | `program_info/prismlauncher.ico -> program_info/sollauncher.ico` | |
| renamed | `program_info/prismlauncher.manifest.in -> program_info/sollauncher.manifest.in` | |
| renamed | `program_info/prismlauncher.qrc.in -> program_info/sollauncher.qrc.in` | |
| renamed | `program_info/prismlauncher.rc.in -> program_info/sollauncher.rc.in` | |
| added | `program_info/io.github.solfain.SOLLauncher.metainfo.xml.in` | |
| deleted | `program_info/org.prismlauncher.PrismLauncher.metainfo.xml.in` | |

## 2. Add the SOL iceberg logo and application icons

| Change | File | |
| --- | --- | --- |
| modified | `launcher/resources/multimc/scalable/launcher.svg` | |
| deleted | `program_info/SOLLauncher.icon/Assets/block.svg` | |
| deleted | `program_info/SOLLauncher.icon/Assets/rainbow.svg` | |
| added | `program_info/SOLLauncher.icon/Assets/body.svg` | |
| added | `program_info/SOLLauncher.icon/Assets/tip.svg` | |
| modified | `program_info/SOLLauncher.icon/icon.json` | |
| added | `program_info/gen_sol_icons.py` | |
| added | `program_info/io.github.solfain.SOLLauncher.logo-darkmode.svg` | |
| added | `program_info/io.github.solfain.SOLLauncher.logo.svg` | |
| modified | `program_info/io.github.solfain.SOLLauncher.svg` | |
| modified | `program_info/io.github.solfain.SOLLauncher_256.png` | |
| modified | `program_info/sollauncher.icns` | |
| modified | `program_info/sollauncher.ico` | |
| deleted | `program_info/org.prismlauncher.PrismLauncher.Social.svg` | |
| deleted | `program_info/org.prismlauncher.PrismLauncher.Source.svg` | |
| deleted | `program_info/org.prismlauncher.PrismLauncher.bigsur.svg` | |
| deleted | `program_info/org.prismlauncher.PrismLauncher.logo-darkmode.svg` | |
| deleted | `program_info/org.prismlauncher.PrismLauncher.logo.source.svg` | |
| deleted | `program_info/org.prismlauncher.PrismLauncher.logo.svg` | |
| deleted | `program_info/prismlauncher-monochrome.Source.svg` | |

## 3. Add the SOL Light and SOL Dark themes

| Change | File | |
| --- | --- | --- |
| modified | `launcher/Application.cpp` | |
| modified | `launcher/CMakeLists.txt` | |
| modified | `launcher/main.cpp` | |
| added | `launcher/resources/sol/dark/check-disabled.svg` | |
| added | `launcher/resources/sol/dark/check.svg` | |
| added | `launcher/resources/sol/dark/chevron-down-disabled.svg` | |
| added | `launcher/resources/sol/dark/chevron-down.svg` | |
| added | `launcher/resources/sol/dark/chevron-right.svg` | |
| added | `launcher/resources/sol/dark/chevron-up-disabled.svg` | |
| added | `launcher/resources/sol/dark/chevron-up.svg` | |
| added | `launcher/resources/sol/dark/indeterminate.svg` | |
| added | `launcher/resources/sol/dark/radio-dot-disabled.svg` | |
| added | `launcher/resources/sol/dark/radio-dot.svg` | |
| added | `launcher/resources/sol/light/check-disabled.svg` | |
| added | `launcher/resources/sol/light/check.svg` | |
| added | `launcher/resources/sol/light/chevron-down-disabled.svg` | |
| added | `launcher/resources/sol/light/chevron-down.svg` | |
| added | `launcher/resources/sol/light/chevron-right.svg` | |
| added | `launcher/resources/sol/light/chevron-up-disabled.svg` | |
| added | `launcher/resources/sol/light/chevron-up.svg` | |
| added | `launcher/resources/sol/light/indeterminate.svg` | |
| added | `launcher/resources/sol/light/radio-dot-disabled.svg` | |
| added | `launcher/resources/sol/light/radio-dot.svg` | |
| added | `launcher/resources/sol/sol.qrc` | |
| added | `launcher/resources/sol/sol.qss.in` | |
| added | `launcher/resources/sol/tokens.json` | |
| added | `launcher/ui/themes/SolTheme.cpp` | |
| added | `launcher/ui/themes/SolTheme.h` | |
| modified | `launcher/ui/themes/ThemeManager.cpp` | |
| modified | `launcher/ui/widgets/PageContainer.cpp` | |
| modified | `launcher/ui/widgets/VersionListView.cpp` | |
| modified | `tests/CMakeLists.txt` | |
| added | `tests/SolTheme_test.cpp` | |

## 4. Restructure the main window: sidebar navigation, page header and instance cards

| Change | File | |
| --- | --- | --- |
| modified | `launcher/CMakeLists.txt` | |
| modified | `launcher/ui/MainWindow.cpp` | |
| modified | `launcher/ui/MainWindow.h` | |
| modified | `launcher/ui/MainWindow.ui` | |
| modified | `launcher/ui/instanceview/InstanceDelegate.cpp` | |
| modified | `launcher/ui/instanceview/InstanceView.cpp` | |
| modified | `launcher/ui/instanceview/InstanceView.h` | |
| modified | `launcher/ui/instanceview/VisualGroup.cpp` | |
| added | `launcher/ui/widgets/SidebarButton.cpp` | |
| added | `launcher/ui/widgets/SidebarButton.h` | |
| modified | `launcher/ui/widgets/WideBar.cpp` | |

## 5. Add SOL Launcher documentation

| Change | File | |
| --- | --- | --- |
| modified | `README.md` | |
| added | `docs/sol/BUILDING.md` | |
| added | `docs/sol/CHANGES.md` | |
| added | `docs/sol/DESIGN.md` | |
| added | `docs/sol/FORK_NOTES.md` | |
| added | `docs/sol/screenshots/empty-state-light.png` | |
| added | `docs/sol/screenshots/main-dark.png` | |
| added | `docs/sol/screenshots/main-light.png` | |
| added | `docs/sol/screenshots/settings-dark.png` | |
| added | `docs/sol/screenshots/settings-light.png` | |

## Behaviour-adjacent changes

Nothing in the launching, account, instance, modpack, Java or settings *logic* was rewritten. These are the changes you
can observe at run time, so you can decide whether you want each of them:

1. **API keys are blank.** The Microsoft client ID, the CurseForge key and the Imgur client ID are not shipped.
   Without them: *Add Microsoft account* is hidden (so no online play), CurseForge browsing is off, screenshot upload is
   hidden. Supply your own with `-DLauncher_MSA_CLIENT_ID=...` etc. or in **Settings > APIs**. See `FORK_NOTES.md`.
2. **Own data folder and names.** `SOLLauncher` data folder, `sollauncher.cfg`, `SOLLAUNCHER_DATA_DIR` /
   `SOLLAUNCHER_JAVA_PATHS`, binary `sollauncher`, application id `io.github.solfain.SOLLauncher`. Prism Launcher's data is
   never read or written, so both can be installed side by side (they do not share instances or accounts).
3. **URL schemes.** `prismlauncher://` is no longer registered; `sollauncher://` is. The installer and desktop entry
   still register `curseforge://` and `modrinth://`, so installing SOL takes those over from an installed Prism Launcher.
4. **No self-update, news or chat links.** The updater is not enabled (it needs `Launcher_BUILD_ARTIFACT`), the macOS
   Sparkle feed is blank, the news feed is empty (the bar is hidden) and the Discord / Matrix / Reddit Help entries are
   hidden. *Launcher Help* opens this repository's README; the bug tracker points at this repository.
5. **Prism-hosted services are still used** for Minecraft / loader metadata, legacy Forge libraries, translations and
   the in-dialog help pages, because the launcher cannot work without a metadata source.
6. **First-run defaults.** The theme defaults to SOL Light or SOL Dark (the OS colour scheme only picks which) and the
   icon set to the flat one, on every platform. Existing themes remain selectable.
7. **The main toolbar is now a fixed left sidebar.** It cannot be dragged to another edge any more, so *Lock toolbars*
   now only governs the instance and news toolbars. *Replace toolbar with menubar* still works.
8. **The cat toggle shows its "Meow" label** in the sidebar (it used to be an icon-only toolbar button).
9. **Custom painting of the instance view.** Cards, group headers, hover and the empty state are drawn by new code.
   Selection, drag and drop, rename, collapsing and context menus use the unchanged upstream logic, but see the
   verification list below for what was actually exercised.
10. **"No versions are currently available"** in the version lists is now muted centred text instead of a black overlay.

## Verification

Done in a sandbox: Linux x86_64, Clang 22 with libc++ (Zig toolchain), Qt 6.8.5 built from source, UI driven
headless through Qt's VNC platform plugin.

- A clean full build under `-Wall -Wextra -Wpedantic -Werror`: no errors and no warnings.
- **24 of 24 test suites pass**: the 23 existing ones plus the new `SolTheme` suite (13 cases: token completeness,
  no unresolved placeholders, every referenced image exists, palette matches tokens, WCAG contrast, low chroma).
- The real application was started and used in the light and the dark theme: first-run wizard, empty and populated
  library, hover / selection / group collapse / context menu, the Folders, Help and Accounts menus, Settings, the About
  dialog (About and Credits tabs), the New Instance dialog and the instance editor.
- Colour values in the screenshots were compared with the palette tokens pixel for pixel.

**Not tested** (no way to do it in the sandbox, or out of scope without your keys):

- the Windows build (MSVC, vcpkg presets, `cmake --install` deployment) and the NSIS installer; the macOS build
  (including the new icon bundle); GCC 13+ builds; the Java helper jars (`libraries/launcher`, `libraries/javacheck`)
  because no JDK was available;
- Microsoft / Minecraft sign-in and online play (needs your own client ID and Mojang's approval), CurseForge, and the
  Modrinth / FTB / ATLauncher / Technic network flows (the sandbox has no access to those hosts);
- actually launching Minecraft, Java download and detection on a real system;
- high-DPI scaling and Segoe UI rendering on Windows (screenshots use Open Sans), and the Linux desktop integration
  files (`.desktop`, AppStream, man page) beyond their build step;
- the self-updater, and behaviour against the Prism-hosted metadata and help services.
