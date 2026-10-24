# Smoke test of a packaged Windows build: is everything a Qt application with SVG icons and HTTPS needs in the folder, does the
# executable start, and does it stay up. Optionally also runs the NSIS installer silently and checks the installed copy.
#
# The result is one annotation ("notice" when all is well, "warning" otherwise) so it can be read from the run page and the API,
# and the script fails (exit code 1) only when the program does not run at all.
#
# usage: .github/scripts/smoke-test-windows.ps1 -Dir install-portable [-Installer SOLLauncher-Setup.exe]
param(
    [Parameter(Mandatory = $true)][string]$Dir,
    [string]$Installer = ''
)

function Escape([string]$text) {
    return (($text -replace '%', '%25') -replace "`r", '%0D') -replace "`n", '%0A'
}

# Checks one folder that contains sollauncher.exe. Returns the report lines; sets $script:failed when the program does not run.
function Test-Build([string]$Folder, [string]$Label) {
    $lines = New-Object System.Collections.Generic.List[string]
    $lines.Add("== $Label ($Folder)")

    $required = @(
        'sollauncher.exe', 'sollauncher_filelink.exe', 'Qt6Core.dll', 'Qt6Gui.dll', 'Qt6Widgets.dll', 'Qt6Svg.dll',
        'platforms\qwindows.dll', 'imageformats\qsvg.dll', 'iconengines\qsvgicon.dll', 'jars\NewLaunch.jar', 'jars\JavaCheck.jar'
    )
    $missing = @($required | Where-Object { -not (Test-Path (Join-Path $Folder $_)) })
    $lines.Add(('required files present: {0} of {1}' -f ($required.Count - $missing.Count), $required.Count))
    foreach ($name in $missing) { $lines.Add("MISSING: $name") }
    if ($missing.Count -gt 0) { $script:incomplete = $true }

    $tls = @(Get-ChildItem (Join-Path $Folder 'tls') -Filter *.dll -ErrorAction SilentlyContinue | ForEach-Object { $_.Name })
    $lines.Add(('tls plugins: {0}' -f ($tls -join ', ')))
    $size = (Get-ChildItem $Folder -Recurse -File | Measure-Object Length -Sum).Sum / 1MB
    $lines.Add(('{0:N0} MB in {1} files' -f $size, @(Get-ChildItem $Folder -Recurse -File).Count))

    $exe = Join-Path $Folder 'sollauncher.exe'
    if (-not (Test-Path $exe)) {
        $script:failed = $true
        $lines.Add('sollauncher.exe is not there')
        return $lines
    }

    $version = (& $exe --version 2>&1 | Out-String).Trim()
    $versionCode = $LASTEXITCODE
    $lines.Add("--version: exit code $versionCode, '$version'")
    if ($versionCode -ne 0) { $script:failed = $true }

    # start it for real with a scratch data folder and see that it stays up
    $data = Join-Path $env:RUNNER_TEMP ('sol-smoke-' + $Label)
    New-Item -ItemType Directory -Force $data | Out-Null
    $process = Start-Process -FilePath $exe -ArgumentList @('--dir', $data) -PassThru
    Start-Sleep -Seconds 12
    if ($process.HasExited) {
        $lines.Add(('STARTUP FAILED: the process exited by itself with code {0}' -f $process.ExitCode))
        $script:failed = $true
    } else {
        $lines.Add('startup: still running after 12 s')
        Stop-Process -Id $process.Id -Force
    }
    $logs = @(Get-ChildItem (Join-Path $data 'logs') -Filter *.log -ErrorAction SilentlyContinue)
    if ($logs.Count -gt 0) {
        $lines.Add("log ($($logs[0].Name)), last lines:")
        foreach ($line in @(Get-Content $logs[0].FullName -Tail 8)) { $lines.Add('  ' + $line) }
    } else {
        $lines.Add('no log file was written')
    }
    return $lines
}

$script:failed = $false
$script:incomplete = $false
$report = New-Object System.Collections.Generic.List[string]

foreach ($line in (Test-Build $Dir 'portable')) { $report.Add($line) }

if ($Installer -ne '') {
    $target = Join-Path $env:RUNNER_TEMP 'sol-installed'
    # NSIS: /S is silent, /D sets the folder and must be the last argument, unquoted
    $install = Start-Process -FilePath (Resolve-Path $Installer).Path -ArgumentList @('/S', "/D=$target") -PassThru
    if (-not $install.WaitForExit(180000)) {
        $report.Add('INSTALLER: still running after 3 minutes, stopped')
        Stop-Process -Id $install.Id -Force
        $script:failed = $true
    } else {
        $report.Add(('installer: silent install finished with exit code {0}' -f $install.ExitCode))
        if ($install.ExitCode -ne 0) { $script:failed = $true }
        foreach ($line in (Test-Build $target 'installed')) { $report.Add($line) }
    }
}

$level = if ($script:failed -or $script:incomplete) { 'warning' } else { 'notice' }
Write-Host ('::{0} title=Smoke test of the packaged build::{1}' -f $level, (Escape ($report -join "`n")))
if ($script:failed) { exit 1 }
