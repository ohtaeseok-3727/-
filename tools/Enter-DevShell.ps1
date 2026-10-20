# Dot-source this file to prepare the current terminal; this never builds or runs the game.
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
if (-not (Test-Path -LiteralPath $vswhere)) { throw 'Visual Studio Installer is missing.' }
$installation = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $installation) { throw 'Install the Visual Studio Desktop development with C++ workload.' }
Import-Module (Join-Path $installation 'Common7\Tools\Microsoft.VisualStudio.DevShell.dll')
Enter-VsDevShell -VsInstallPath $installation -SkipAutomaticLocation -DevCmdArguments '-arch=x64 -host_arch=x64'
# Prefer tools already on PATH. Fall back to the existing Codex bundled tools.
$bundled = Join-Path $env:USERPROFILE '.cache\codex-runtimes\codex-primary-runtime\dependencies'
foreach ($tool in @(@{ Name = 'git'; Relative = 'native\git\cmd' }, @{ Name = 'python'; Relative = 'python' }))
{
    $existing = Get-Command $tool.Name -ErrorAction SilentlyContinue
    if (-not $existing -or $existing.Source -like '*\Microsoft\WindowsApps\*')
    {
        $directory = Join-Path $bundled $tool.Relative
        if (Test-Path -LiteralPath (Join-Path $directory ($tool.Name + '.exe')))
        {
            $env:PATH = "$directory;$env:PATH"
        }
        else { Write-Warning "$($tool.Name) is unavailable; install it separately if needed." }
    }
}
Set-Location -LiteralPath $projectRoot
Write-Host 'Development terminal ready (x64). No build or game launch was performed.'
