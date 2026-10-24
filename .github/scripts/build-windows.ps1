# Builds with the given CMake preset on a Windows runner, and explains itself if the build dies.
#
# Why this exists: a build once stopped with "exit code 127" and a log that ended in the middle of a line, with nothing in the
# raw log to say why. Git Bash turns a Windows NTSTATUS into such a small number, so this runs under PowerShell, which shows
# the real one, samples the memory of the machine while the compiler runs (a hosted Windows runner has about 16 GB, and Qt-heavy
# translation units need a lot of it) and, on failure, repeats the evidence as an error annotation: the end of the build log,
# the last memory samples, and the Windows Application event log of the last minutes (a crashed process shows up there).
#
# usage: .github/scripts/build-windows.ps1 -Preset windows_msvc -Config Release -Parallel 3
param(
    [Parameter(Mandatory = $true)][string]$Preset,
    [Parameter(Mandatory = $true)][string]$Config,
    [int]$Parallel = 0
)

function Escape([string]$text) {
    return (($text -replace '%', '%25') -replace "`r", '%0D') -replace "`n", '%0A'
}

$log = Join-Path $env:RUNNER_TEMP 'build.log'

# every 5 seconds: free physical memory, free virtual memory, and the three biggest processes
$sampler = Start-Job -ScriptBlock {
    while ($true) {
        $os = Get-CimInstance Win32_OperatingSystem
        $top = Get-Process | Sort-Object WorkingSet64 -Descending | Select-Object -First 3 | ForEach-Object { '{0} {1:N1}G' -f $_.ProcessName, ($_.WorkingSet64 / 1GB) }
        '{0}  physical free {1:N1} of {2:N1} GB, virtual free {3:N1} of {4:N1} GB  |  {5}' -f (Get-Date -Format 'HH:mm:ss'), ($os.FreePhysicalMemory / 1MB), ($os.TotalVisibleMemorySize / 1MB), ($os.FreeVirtualMemory / 1MB), ($os.TotalVirtualMemorySize / 1MB), ($top -join ', ')
        Start-Sleep -Seconds 5
    }
}

$arguments = @('--build', '--preset', $Preset, '--config', $Config)
if ($Parallel -gt 0) { $arguments += @('--parallel', $Parallel) }
$started = Get-Date
& cmake @arguments 2>&1 | Tee-Object -FilePath $log
$code = $LASTEXITCODE

Stop-Job $sampler
$samples = @(Receive-Job $sampler)
Remove-Job $sampler -Force

$hex = '0x{0:X8}' -f [BitConverter]::ToUInt32([BitConverter]::GetBytes([int]$code), 0)
if ($code -ne 0) {
    $parts = @()
    $parts += "exit code $code ($hex), after {0:N0} s" -f ((Get-Date) - $started).TotalSeconds
    $parts += '--- end of the build log'
    $parts += @(Get-Content $log -Tail 25)
    $parts += '--- memory samples (last 12)'
    $parts += @($samples | Select-Object -Last 12)
    $parts += '--- Application event log, errors of the last 30 minutes'
    try {
        $events = Get-WinEvent -FilterHashtable @{ LogName = 'Application'; Level = 1, 2; StartTime = (Get-Date).AddMinutes(-30) } -MaxEvents 8 -ErrorAction Stop
        foreach ($event in $events) {
            $first = ($event.Message -split "`r?`n" | Select-Object -First 3) -join ' / '
            $parts += '{0} {1} id {2}: {3}' -f $event.TimeCreated.ToString('HH:mm:ss'), $event.ProviderName, $event.Id, $first
        }
    } catch {
        $parts += "(no events: $($_.Exception.Message))"
    }
    Write-Host ("::error title=Build failed ({0})::{1}" -f $hex, (Escape ($parts -join "`n")))
} elseif ($samples.Count -gt 0) {
    # on success the lowest free memory is still worth knowing: it says how close the build comes to the limit
    $lowest = $samples | Sort-Object { [double](($_ -split 'physical free ')[1] -split ' ')[0] } | Select-Object -First 1
    Write-Host ("::notice title=Lowest free memory during the build::{0}" -f (Escape $lowest))
}
exit $code
