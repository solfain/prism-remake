# Changes compared to the Prism Launcher baseline

The first commit on this branch (`3012098`, *Import Prism Launcher develop as the SOL Launcher baseline*) is an
unmodified snapshot of Prism Launcher `develop`. Everything after it is SOL Launcher. This page is generated from git;
for the full diff run `git diff 3012098..HEAD`. Net result: 264 files changed, 6239 insertions(+), 2237 deletions(-).

## Commits

| # | Commit | What it is |
| --- | --- | --- |
| 1 | Rebrand to SOL Launcher: identity, API keys, services and branding strings | Product identity: names, ids, data folder, version, API keys and service URLs, branding strings, About / license text |
| 2 | Add the SOL iceberg logo and application icons | The first (iceberg) logo and every application icon format, plus the generator script |
| 3 | Add the SOL Light and SOL Dark themes | The SOL Light / SOL Dark themes (palette, stylesheet, `SolTheme`) and their unit tests |
| 4 | Restructure the main window: sidebar navigation, page header and instance cards | Sidebar navigation, page header, instance cards, group headers, empty state, instance panel styling |
| 5 | Add SOL Launcher documentation | README, `docs/sol/*`, screenshots |
| 6 | Add CI: build and test on Windows (MSVC) and Linux (GCC), publish a Windows preview | GitHub Actions: build and test on Windows (MSVC) and Linux (GCC), publish a Windows installer / portable zip as the *preview* release |
| 7 | Simplify the logo: a diamond split at the waterline | The logo becomes one shape, a diamond split at the waterline; four alternates behind a flag |
| 8 | Add the SOL line icon set, which follows the active theme | 45 thin line icons (Lucide) that follow the active theme (adaptive light / dark strokes) |
| 9 | Fix the Windows installer build and make CI easier to debug | NSIS no longer requires the updater executable; `.ico` entry order; cache and key variables in CI |
| 10 | Microsoft sign-in: never a dead end, guided setup, one-click entry | Guided setup for Microsoft sign-in (OAuth), a direct menu entry, CI variable for the client ID, tests |
| 11 | Animate the instance cards and scroll smoothly | Eased hover / selection on the cards, smooth mouse-wheel scrolling |
| 12 | Fix the theme problems a walk through every page turned up | Invisible group-box and menu check marks, framed scroll areas, Launch button, fatal-log colours, and more |
| 13 | CI: report failing steps as annotations | CI failures are repeated as annotations (readable without the raw logs) |
| 14 | Message boxes use the SOL line icons | Message box icons come from the SOL icon set |
| 15 | Let a build choose its version channel; CI builds say "preview" | `Launcher_VERSION_CHANNEL`; CI builds read `1.0.0-preview` |
| 16 | CI: keep the Windows build from running out of disk |  |
| 17 | Credit Lucide in the app, document the new look, retake the screenshots | Lucide credited in About > Credits; docs and screenshots refreshed; CHANGES organised by area |
| 18 | About dialog: Close is the default button, not "About Qt" | Close, not "About Qt", is the default button |
| 19 | CI: find out why the Windows build dies, and give it less to chew on at once | PowerShell build wrapper that shows the real exit status and the failing command |
| 20 | README: describe the new logo |  |
| 21 | Window frames follow the SOL theme instead of the system setting | Title bars follow the SOL theme (Windows, macOS, desktop Linux) instead of the system setting |
| 22 | MSVC: whole-program optimization only when ENABLE_LTO asks for it | (reverted in a later commit: the diagnosis was wrong) |
| 23 | Docs: ENABLE_LTO and the other small deviations from upstream's build | Docs for the deviations from upstream's build |
| 24 | Fix the Windows link error (MsaSetup_test's main) and revert the LTO change that misdiagnosed it | The real cause of the failing Windows build: an MSVC-only link error in the MsaSetup test; the wrong LTO change reverted |
| 25 | Add two test data files that .gitignore kept out of the baseline import | Two upstream test files that .gitignore had kept out of the baseline import |
| 26 | CI: undo the disk-space workarounds (the junction broke upstream's FileSystem test) | Removed the disk-space workarounds (the build-dir junction broke upstream's FileSystem test) |
| 27 | CI: smoke-test the packaged Windows build (files, start-up, silent install) | Smoke test of the packaged Windows build: files, start-up, silent install of the installer |
| 28 | README and BUILDING: the preview download, and how the CI works | Download links for the preview release, and how the CI works |

## Files changed, by area

Net change of each file against the baseline (a file touched by several commits appears once).

### Continuous integration (4)

| Change | File | |
| --- | --- | --- |
| added | `.github/scripts/build-windows.ps1` | |
| added | `.github/scripts/ci-run.sh` | |
| added | `.github/scripts/smoke-test-windows.ps1` | |
| added | `.github/workflows/build.yml` | |

### Tests (5)

| Change | File | |
| --- | --- | --- |
| modified | `tests/CMakeLists.txt` | |
| added | `tests/MsaSetup_test.cpp` | |
| added | `tests/SolTheme_test.cpp` | |
| added | `tests/testdata/DataPackParse/another_test_folder/data/dummy/tags/item/foo_proof/bar.json` | |
| added | `tests/testdata/DataPackParse/test_folder/data/dummy/tags/item/foo_proof/bar.json` | |

### Documentation (12)

| Change | File | |
| --- | --- | --- |
| modified | `README.md` | |
| added | `docs/sol/BUILDING.md` | |
| added | `docs/sol/CHANGES.md` | |
| added | `docs/sol/DESIGN.md` | |
| added | `docs/sol/FORK_NOTES.md` | |
| added | `docs/sol/MICROSOFT_SIGN_IN.md` | |
| added | `docs/sol/screenshots/empty-state-light.png` | |
| added | `docs/sol/screenshots/main-dark.png` | |
| added | `docs/sol/screenshots/main-light.png` | |
| added | `docs/sol/screenshots/msa-setup-light.png` | |
| added | `docs/sol/screenshots/settings-dark.png` | |
| added | `docs/sol/screenshots/settings-light.png` | |

### Logo and application icons (25)

| Change | File | |
| --- | --- | --- |
| modified | `launcher/resources/multimc/scalable/launcher.svg` | |
| deleted | `program_info/PrismLauncher.icon/Assets/block.svg` | |
| deleted | `program_info/PrismLauncher.icon/Assets/rainbow.svg` | |
| deleted | `program_info/PrismLauncher.icon/icon.json` | |
| added | `program_info/SOLLauncher.icon/Assets/body.svg` | |
| added | `program_info/SOLLauncher.icon/Assets/tip.svg` | |
| added | `program_info/SOLLauncher.icon/icon.json` | |
| added | `program_info/gen_sol_icons.py` | |
| added | `program_info/io.github.solfain.SOLLauncher.logo-darkmode.svg` | |
| added | `program_info/io.github.solfain.SOLLauncher.logo.svg` | |
| added | `program_info/io.github.solfain.SOLLauncher.svg` | |
| added | `program_info/io.github.solfain.SOLLauncher_256.png` | |
| added | `program_info/sollauncher.icns` | |
| added | `program_info/sollauncher.ico` | |
| deleted | `program_info/` | 11 files (.icns, .ico, .png, .svg) |

### Line icon set (140)

| Change | File | |
| --- | --- | --- |
| added | `launcher/resources/sol_icons/LICENSE-lucide.txt` | |
| added | `launcher/resources/sol_icons/generate_icons.py` | |
| added | `launcher/resources/sol_icons/sol_icons.qrc` | |
| added | `launcher/resources/sol_icons/lucide/` | 45 files (.svg) |
| added | `launcher/resources/sol_icons/sol/index.theme` | |
| added | `launcher/resources/sol_icons/sol/scalable/` | 45 files (.svg) |
| added | `launcher/resources/sol_icons/sol_white/index.theme` | |
| added | `launcher/resources/sol_icons/sol_white/scalable/` | 45 files (.svg) |

### Themes (palette, stylesheet, SolTheme) (35)

| Change | File | |
| --- | --- | --- |
| added | `launcher/resources/sol/dark/` | 12 files (.svg) |
| added | `launcher/resources/sol/light/` | 12 files (.svg) |
| added | `launcher/resources/sol/sol.qrc` | |
| added | `launcher/resources/sol/sol.qss.in` | |
| added | `launcher/resources/sol/tokens.json` | |
| modified | `launcher/ui/themes/ITheme.cpp` | |
| modified | `launcher/ui/themes/ITheme.h` | |
| modified | `launcher/ui/themes/IconTheme.cpp` | |
| modified | `launcher/ui/themes/IconTheme.h` | |
| modified | `launcher/ui/themes/ThemeManager.cpp` | |
| modified | `launcher/ui/themes/ThemeManager.h` | |
| added | `launcher/ui/themes/SolTheme.cpp` | |
| added | `launcher/ui/themes/SolTheme.h` | |

### Main window, instance view and widgets (13)

| Change | File | |
| --- | --- | --- |
| modified | `launcher/ui/InstanceWindow.cpp` | |
| modified | `launcher/ui/MainWindow.cpp` | |
| modified | `launcher/ui/MainWindow.h` | |
| modified | `launcher/ui/MainWindow.ui` | |
| modified | `launcher/ui/instanceview/InstanceDelegate.cpp` | |
| modified | `launcher/ui/instanceview/InstanceView.cpp` | |
| modified | `launcher/ui/instanceview/InstanceView.h` | |
| modified | `launcher/ui/instanceview/VisualGroup.cpp` | |
| modified | `launcher/ui/widgets/PageContainer.cpp` | |
| modified | `launcher/ui/widgets/VersionListView.cpp` | |
| modified | `launcher/ui/widgets/WideBar.cpp` | |
| added | `launcher/ui/widgets/SidebarButton.cpp` | |
| added | `launcher/ui/widgets/SidebarButton.h` | |

### Microsoft sign-in (3)

| Change | File | |
| --- | --- | --- |
| added | `launcher/ui/dialogs/MsaSetupDialog.cpp` | |
| added | `launcher/ui/dialogs/MsaSetupDialog.h` | |
| modified | `launcher/ui/pages/global/AccountListPage.cpp` | |

### Identity, branding and build configuration (27)

| Change | File | |
| --- | --- | --- |
| modified | `CMakeLists.txt` | |
| modified | `COPYING.md` | |
| modified | `buildconfig/BuildConfig.cpp.in` | |
| modified | `cmake/MacOSXBundleInfo.plist.in` | |
| modified | `launcher/Application.cpp` | |
| modified | `launcher/CMakeLists.txt` | |
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
| renamed | `program_info/org.prismlauncher.PrismLauncher.desktop.in -> program_info/io.github.solfain.SOLLauncher.desktop.in` | |
| renamed | `program_info/org.prismlauncher.PrismLauncher.mime.xml -> program_info/io.github.solfain.SOLLauncher.mime.xml` | |
| renamed | `program_info/prismlauncher.6.scd.in -> program_info/sollauncher.6.scd.in` | |
| renamed | `program_info/prismlauncher.manifest.in -> program_info/sollauncher.manifest.in` | |
| renamed | `program_info/prismlauncher.qrc.in -> program_info/sollauncher.qrc.in` | |
| renamed | `program_info/prismlauncher.rc.in -> program_info/sollauncher.rc.in` | |
| added | `program_info/io.github.solfain.SOLLauncher.metainfo.xml.in` | |
| deleted | `program_info/org.prismlauncher.PrismLauncher.metainfo.xml.in` | |

## Behaviour-adjacent changes

Nothing in the launching, account, instance, modpack, Java or settings *logic* was rewritten. These are the changes you
can observe at run time, so you can decide whether you want each of them:

1. **API keys are blank, and Microsoft sign-in needs your own application ID.** The Microsoft client ID, the CurseForge key
   and the Imgur client ID are not shipped. *Add Microsoft account...* is always offered; without an ID it opens a guided
   setup (register an Entra app, paste its ID). Register once, set the `SOL_MSA_CLIENT_ID` repository variable and CI bakes it
   into every build. Mojang must also approve the app. Offline accounts still require a Microsoft account first (upstream's
   rule, kept). See `MICROSOFT_SIGN_IN.md` and `FORK_NOTES.md`.
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
6. **First-run defaults.** The theme defaults to SOL Light or SOL Dark (the OS colour scheme only picks which) and the icon
   theme to *SOL Line*, which follows the theme by itself. Existing configurations keep the icon theme they have, and all
   other themes remain selectable.
7. **The main toolbar is now a fixed left sidebar.** It cannot be dragged to another edge any more, so *Lock toolbars*
   now only governs the instance and news toolbars. *Replace toolbar with menubar* still works.
8. **The cat toggle shows its "Meow" label** in the sidebar (it used to be an icon-only toolbar button).
9. **Custom painting and motion in the instance view.** Cards, group headers, hover and the empty state are drawn by new
   code, hover / selection ease over 140 ms and a classic mouse wheel scrolls with an 180 ms ease. Selection, drag and drop,
   rename, collapsing and context menus use the unchanged upstream logic.
10. **Message boxes** show the SOL line icons (amber warning, red error) instead of the platform's while a SOL icon theme is active.
11. **"No versions are currently available"** in the version lists is now muted centred text instead of a black overlay.
12. **Untagged builds can name their channel.** `Launcher_VERSION_CHANNEL` replaces the git branch after the version
    (`1.0.0-preview`); empty keeps the old behaviour.
13. **CI publishes a rolling "preview" pre-release** from the default branch and `arena/*` branches (unsigned installer and
    portable zip). It creates and replaces the `preview` tag and release; nothing else is published.

## Verification

Done in a sandbox: Linux x86_64, Clang 22 with libc++ (Zig toolchain), Qt 6.8.5 built from source, UI driven
headless through Qt's VNC platform plugin; plus GitHub Actions (see below).

- A clean full build under `-Wall -Wextra -Wpedantic -Werror`: no errors and no warnings.
- **All test suites pass** locally: the existing ones plus `SolTheme` (21 checks: tokens, placeholders, images, palette, WCAG
  contrast, low chroma, the line icon set, fatal-log readability) and `MsaSetup` (19 checks).
- The real application was started and used in the light and the dark theme: first-run wizard, empty and populated
  library, hover / selection / group collapse / context menu, the Folders, Help and Accounts menus, Settings, the About
  dialog, the New Instance dialog, the instance editor, the Microsoft setup dialog, message boxes. The card animation was
  checked by logging its progress (a clean ramp, a cross-fade between cards).
- **GitHub Actions** builds with GCC on Linux and with MSVC on Windows; **all 25 test suites pass on both**. CI found and fixed a
  real MSVC-only link error in a test I had written (`char** argv` against the driver's `char* argv[]`).
- **The packaged Windows build was smoke-tested on a real Windows runner**: all required files are in the package (Qt SVG, the
  windows platform plugin, TLS, both helper jars), `sollauncher.exe --version` works, the program starts and keeps running, and
  the NSIS installer installs silently with exit code 0 and the installed copy starts the same way.

**Not tested** (no way to do it in the sandbox, or out of scope without your keys):

- the **OAuth sign-in itself** (needs your own application ID and Mojang's approval), CurseForge, and the Modrinth / FTB /
  ATLauncher / Technic network flows (the sandbox has no access to those hosts);
- actually launching Minecraft, Java download and detection on a real system;
- how the interface looks on Windows (Segoe UI, high-DPI scaling, dark title bars: screenshots use Open Sans on Linux), and
  clicking through the installer's wizard by hand (only the silent install was run);
- macOS (including the new icon bundle); the Linux desktop integration files (`.desktop`, AppStream, man page) beyond their
  build step;
- the self-updater, and behaviour against the Prism-hosted metadata and help services.
