#Requires -Version 5.1
<#
.SYNOPSIS
    Release build + portable packaging for AKIP/Gratten Generator Manager.

.DESCRIPTION
    Static mode  (default): one standalone EXE + CH375DLL64.dll only.
                             Requires a static Qt build at -StaticQtDir.
    Dynamic mode (-Dynamic): EXE + Qt DLLs bundled via windeployqt.

.PARAMETER StaticQtDir
    Path to static Qt MinGW build. Default: C:\Qt\6.8.0-static\mingw_64
    Build it once with .\build_qt_static.ps1 if not present.

.PARAMETER DynamicQtDir
    Path to dynamic Qt MinGW build. Default: C:\Qt\6.8.0\mingw_64

.PARAMETER MingwDir
    Path to MinGW bin. Default: C:\Qt\Tools\mingw1310_64\bin

.PARAMETER Dynamic
    Switch to dynamic Qt deployment (windeployqt). Use only for debugging.

.EXAMPLE
    .\deploy.ps1                          # static build (default)
    .\deploy.ps1 -Dynamic                 # dynamic build with Qt DLLs
    .\deploy.ps1 -StaticQtDir D:\Qt-static\6.8.0\mingw_64
#>
param(
    [string]$StaticQtDir  = "C:\Qt\6.8.0-static\mingw_64",
    [string]$DynamicQtDir = "C:\Qt\6.8.0\mingw_64",
    [string]$MingwDir     = "C:\Qt\Tools\mingw1310_64\bin",
    [switch]$Dynamic
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$Root     = $PSScriptRoot
$AppName  = "AKIP-manager_module"
$AppExe   = "$AppName.exe"
$Ch375Dll = "CH375DLL64.dll"
$Ch375Src = Join-Path $Root "ch375_sdk\$Ch375Dll"

if ($Dynamic) {
    $QtDir    = $DynamicQtDir
    $BuildDir = Join-Path $Root "build_release"
    $Preset   = "release-dynamic"
} else {
    $QtDir    = $StaticQtDir
    $BuildDir = Join-Path $Root "build_static"
    $Preset   = "release-static"

    if (-not (Test-Path (Join-Path $QtDir "lib\cmake\Qt6\Qt6Config.cmake"))) {
        Write-Error @"
Static Qt not found at: $QtDir

Build it first (one-time, ~60 min):
    .\build_qt_static.ps1

Or point to your static Qt:
    .\deploy.ps1 -StaticQtDir "D:\MyStaticQt\mingw_64"
"@
        exit 1
    }
}

$DeployDir = Join-Path $Root "deploy"

Write-Host ""
Write-Host "=== AKIP/Gratten Manager — portable deploy ===" -ForegroundColor Cyan
Write-Host "Mode:  $(if ($Dynamic) { 'DYNAMIC (Qt DLLs)' } else { 'STATIC (standalone EXE)' })"
Write-Host "Qt:    $QtDir"
Write-Host "MinGW: $MingwDir"
Write-Host "Out:   $DeployDir"
Write-Host ""

$env:PATH = "$QtDir\bin;$MingwDir;C:\Qt\Tools\Ninja;" + $env:PATH

# ── 1. Configure ─────────────────────────────────────────────────────────────
Write-Host "[1/3] CMake configure ($Preset)..." -ForegroundColor Yellow
New-Item -ItemType Directory -Force $BuildDir | Out-Null

cmake $Root `
    --preset $Preset `
    -DCMAKE_PREFIX_PATH="$QtDir" `
    -DCMAKE_CXX_COMPILER="$MingwDir\g++.exe"
if ($LASTEXITCODE -ne 0) { Write-Error "CMake configure failed"; exit 1 }

# ── 2. Build ─────────────────────────────────────────────────────────────────
Write-Host "[2/3] Building Release..." -ForegroundColor Yellow
cmake --build $BuildDir --config Release --parallel
if ($LASTEXITCODE -ne 0) { Write-Error "Build failed"; exit 1 }

$ExeBuilt = Join-Path $BuildDir $AppExe
if (-not (Test-Path $ExeBuilt)) {
    Write-Error "Binary not found: $ExeBuilt"
    exit 1
}

# ── 3. Package ───────────────────────────────────────────────────────────────
Write-Host "[3/3] Packaging deploy folder..." -ForegroundColor Yellow
Remove-Item -Recurse -Force $DeployDir -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force $DeployDir | Out-Null

Copy-Item $ExeBuilt $DeployDir
Write-Host "  + $AppExe"

# Translations
$TransDir = Join-Path $DeployDir "translations"
New-Item -ItemType Directory -Force $TransDir | Out-Null
foreach ($qm in @("ru_RU.qm", "en_US.qm")) {
    $src = Join-Path $BuildDir $qm
    if (Test-Path $src) {
        Copy-Item $src $TransDir
        Write-Host "  + translations\$qm"
    }
}

# CH375 driver DLL (always needed — hardware driver, not Qt)
if (Test-Path $Ch375Src) {
    Copy-Item $Ch375Src $DeployDir
    Write-Host "  + $Ch375Dll  (USB driver, required)"
} else {
    Write-Warning "  CH375DLL64.dll not found at $Ch375Src"
}

if ($Dynamic) {
    # windeployqt bundles all Qt DLLs
    $windeploy = Join-Path $DynamicQtDir "bin\windeployqt.exe"
    & $windeploy --release --no-translations --no-system-d3d-compiler --no-opengl-sw `
        (Join-Path $DeployDir $AppExe)
    if ($LASTEXITCODE -ne 0) { Write-Error "windeployqt failed"; exit 1 }
    Write-Host "  + Qt DLLs (windeployqt)"
}

# ── Summary ──────────────────────────────────────────────────────────────────
$files = Get-ChildItem $DeployDir -Recurse -File
$sizeMb = [math]::Round(($files | Measure-Object -Property Length -Sum).Sum / 1MB, 1)

Write-Host ""
Write-Host "=== Done! ===" -ForegroundColor Green
Write-Host "Files: $($files.Count)   Size: $sizeMb MB"
Write-Host "Path:  $DeployDir"
Write-Host ""
if (-not $Dynamic) {
    Write-Host "Contents (standalone — no Qt install required on target machine):"
    Write-Host "  $AppExe"
    Write-Host "  $Ch375Dll      <- USB driver for AKIP-3417"
    Write-Host "  translations\  <- UI language files"
}
