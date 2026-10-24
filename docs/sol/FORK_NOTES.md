# Fork notes

How SOL Launcher relates to Prism Launcher, and the decisions that follow from that.

## Relationship to Prism Launcher

SOL Launcher is a **derivative work** of [Prism Launcher](https://github.com/PrismLauncher/PrismLauncher)
(GPL-3.0-only), which is itself derived from PolyMC and MultiMC.

- **Base:** Prism Launcher branch `develop` at commit `6f5723937f558b06eb6795d16e0994299ba9c040` (2026-10-23,
  "refactor: remove dead code (#6263)"), which identifies itself as version 12.0.0.
- **The first commit on the SOL branch is an unmodified snapshot** of that tree ("Import Prism Launcher develop as the
  SOL Launcher baseline"). `git diff <that commit>..HEAD` therefore shows exactly what SOL Launcher changed; a summary is
  in [CHANGES.md](CHANGES.md).
- **License:** GPL-3.0-only for the code. The original copyright notices in every source file are untouched, and
  `COPYING.md` (shown in the About dialog) keeps the Prism Launcher / PolyMC / MultiMC notices and adds SOL's.
- **Identity:** the product is called "SOL Launcher". It is not Prism Launcher and is not endorsed by or affiliated with
  the Prism Launcher project; the README, the About dialog, the license tab, the desktop entry, the AppStream metadata
  and the man page all say so. Prism's logo and icons are not used (the diamond-and-waterline mark is new; the only remaining Prism
  logo is the "Prism Launcher" entry in the instance-icon picker, which exists so imported Prism instances keep their
  icon).

Upstream's README asks derivative works to (1) make clear they are not Prism Launcher and (2) change Prism's API keys to
their own or blank them. Both are done.

## API keys

Prism Launcher's Microsoft client ID, CurseForge key and Imgur client ID were issued to Prism Launcher; CurseForge
explicitly requires derivative works to use their own key. SOL Launcher therefore ships **without** them. The code
paths are untouched, and the program degrades gracefully:

| Key | Without it | How to provide it |
| --- | --- | --- |
| Microsoft client ID | no account can be added (offline accounts need a Microsoft one first); "Add Microsoft account" opens a guided setup instead of failing | the `SOL_MSA_CLIENT_ID` CI variable, `-DLauncher_MSA_CLIENT_ID=...`, the in-app setup, or **Settings > APIs** |
| CurseForge API key | CurseForge browsing and pack lookups are disabled | `-DLauncher_CURSEFORGE_API_KEY=...` or **Settings > APIs** |
| Imgur client ID | the screenshot "Upload" action is hidden | `-DLauncher_IMGUR_CLIENT_ID=...` |

### Getting a Microsoft client ID

See [MICROSOFT_SIGN_IN.md](MICROSOFT_SIGN_IN.md): registering the application (personal accounts, public client, the
redirect URIs `http://localhost` and `sollauncher://oauth/microsoft`, public client flows on), Mojang's approval
(<https://aka.ms/AppRegInfo>), and how to ship the ID in every build. The second redirect URI is derived from the binary
name, so it differs from Prism Launcher's, which is another reason a Prism client ID cannot simply be reused.

If you have the Prism Launcher team's permission to reuse their registrations and want to restore the old defaults, copy
the three values from the upstream `CMakeLists.txt` into your configure command line. That is deliberately not the
default here.

## Services

Everything that points outside the launcher is a CMake variable (see the top-level `CMakeLists.txt`).

| Service | Used for | SOL default |
| --- | --- | --- |
| `meta.prismlauncher.org` | Minecraft / loader version lists (the launcher cannot work without a metadata source) | **kept**, run by the Prism Launcher project |
| `files.prismlauncher.org` | libraries for legacy Forge (1.5.2 and older) | **kept** |
| `prismlauncher.org/successful-login` | static page the browser lands on after a Microsoft sign-in | **kept** (shows Prism branding; host your own and set `Launcher_LOGIN_CALLBACK_URL`) |
| `prismlauncher.org/wiki/help-pages` | contextual help links inside dialogs | **kept** |
| `i18n.prismlauncher.org`, Weblate | translation files and the "help translate" link | **kept** (translations are shared) |
| News feed, "More news" | the news bar | **removed** (empty; the bar is not shown) |
| Discord, Matrix, Reddit | Help menu entries | **removed** (empty; the entries are hidden) |
| Launcher Help, bug tracker | Help menu | pointed at this repository |
| GitHub updater, macOS Sparkle | self-update | **not enabled** (needs `Launcher_BUILD_ARTIFACT`, or Sparkle key + feed; the Sparkle values are blank so SOL can never offer to replace itself with a Prism build) |

The kept services are run by the Prism Launcher project, so SOL Launcher depends on their availability and goodwill.
Consider asking them, mirroring what you need, or hosting your own.

## Protocol handlers

The Windows installer and the Linux desktop entry register `curseforge://` and `modrinth://` (one-click modpack
installs from the websites) plus `sollauncher://`. They deliberately do **not** register `prismlauncher://`. Note that
installing SOL Launcher takes over `curseforge://` and `modrinth://` from an installed Prism Launcher, as installing any
second launcher would.

## Data and side-by-side use

The data folder, config file name, log names, organisation / application name and environment variable prefix all derive
from the product name, so SOL Launcher uses `SOLLauncher` folders, `sollauncher.cfg`, and `SOLLAUNCHER_DATA_DIR` /
`SOLLAUNCHER_JAVA_PATHS`, and never reads or writes Prism Launcher's data. (As in upstream, a first run offers to
import data from an old PolyMC / MultiMC folder if one is found.)

Interoperability formats are unchanged on purpose: instance zips and the packwiz `x-prismlauncher-*` metadata keys keep
their names so SOL Launcher and Prism Launcher can exchange instances and modpacks, and the in-game launcher shim keeps
its `org.prismlauncher` Java package.

## Things inherited and untouched

- `flake.nix`, `nix/`, `default.nix`, `shell.nix`, `Containerfile`, `renovate.json`, `.envrc`: upstream packaging and
  tooling for the Prism Launcher package. They still refer to the Prism names and are not maintained for SOL Launcher.
- `.github/`: upstream's CI workflows, issue templates, funding and code owners were **not** imported. SOL has one workflow of
  its own (`.github/workflows/build.yml`, adapted from upstream's `build.yml`): it builds and tests on Windows (MSVC) and Linux
  (GCC), and publishes the Windows installer and a portable zip as the rolling *preview* release. See [BUILDING.md](BUILDING.md).
- `PrismLauncher-Windows-MSVC-Setup-11.1.1.exe` in the repository root is a Prism Launcher installer that predates the
  source. It is 24 MB, unused by the build, and a likely candidate for removal.

## Pulling in upstream changes

Because the baseline is a snapshot (no shared history with upstream), the first sync needs an explicit merge base. This
procedure is a suggestion and has not been exercised yet:

```
git remote add upstream https://github.com/PrismLauncher/PrismLauncher.git
git fetch upstream develop
BASELINE=$(git log --format=%H --grep='Import Prism Launcher develop as the SOL Launcher baseline')
git merge-tree --write-tree --merge-base="$BASELINE" HEAD upstream/develop   # prints a tree id (and any conflicts)
git commit-tree <tree-id> -p HEAD -p upstream/develop -m "Merge upstream Prism Launcher"
```

After that merge commit exists, upstream's history is an ancestor and later syncs are plain `git merge upstream/develop`.
Expect conflicts in `.github/` (keep SOL's own `build.yml`, drop upstream's other files) and real conflicts where SOL changed the same code
(themes, the main window, the instance grid, branding); resolve those in favour of SOL's layout while taking
upstream's behaviour changes.
