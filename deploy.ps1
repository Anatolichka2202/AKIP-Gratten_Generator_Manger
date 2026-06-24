#Requires -Version 5.1
<#
.SYNOPSIS
    Portable Release build + windeployqt packaging for AKIP/Gratten Generator Manager.

.DESCRIPTION
    1. Configures Release build with CMake (MinGW x64)
    2. Compiles the project
    3. Runs windeployqt to bundle all required Qt DLLs
    4. Copies translations (.qm) and creates a self-contained deploy/ folder

.PARAMETER QtDir
    Path to Qt MinGW x64 installation, e.g. C:\Qt\6.8.0\mingw_64

.PARAMETER MingwDir
    Path to MinGW bin, e.g. C:\Qt\Tools\mingw1310_64\bin

.EXAMPLE
    .\deploy.ps1
    .\deploy.ps1 -QtDir "C:\Qt\6.8.0\mingw_64" -MingwDir "C:\Qt\Tools\mingw1310_64\bin"
#>
param(
    [string]$QtDir    = "C:\Qt\6.8.0\mingw_64",
    [string]$MingwDir = "C:\Qt\Tools\mingw1310_64\bin"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$Root      = $PSScriptRoot
$BuildDir  = Join-Path $Root "build_release"
$DeployDir = Join-Path $Root "deploy"
$AppName   = "AKIP-manager_module"
$ExePath   = Join-Path $BuildDir "$AppName.exe"

Write-Host "=== AKIP/Gratten Manager — portable deploy ===" -ForegroundColor Cyan
Write-Host "Qt:    $QtDir"
Write-Host "MinGW: $MingwDir"
Write-Host "Build: $BuildDir"
Write-Host "Out:   $DeployDir"
Write-Host ""

# ── 1. Configure ────────────────────────────────────────────────────────────
Write-Host "[1/4] CMake configure..." -ForegroundColor Yellow
$env:PATH = "$QtDir\bin;$MingwDir;$env:PATH"
New-Item -ItemType Directory -Force $BuildDir | Out-Null

Push-Location $BuildDir
cmake $Root `
    -G "Ninja" `
    -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_PREFIX_PATH="$QtDir" `
    -DCMAKE_CXX_COMPILER="$MingwDir\g++.exe"
if ($LASTEXITCODE -ne 0) { Write-Error "CMake configure failed"; exit 1 }
Pop-Location

# ── 2. Build ─────────────────────────────────────────────────────────────────
Write-Host "[2/4] Building Release..." -ForegroundColor Yellow
cmake --build $BuildDir --config Release --parallel
if ($LASTEXITCODE -ne 0) { Write-Error "Build failed"; exit 1 }

if (-not (Test-Path $ExePath)) {
    Write-Error "Binary not found: $ExePath"
    exit 1
}

# ── 3. Create deploy folder ──────────────────────────────────────────────────
Write-Host "[3/4] windeployqt..." -ForegroundColor Yellow
Remove-Item -Recurse -Force $DeployDir -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force $DeployDir | Out-Null

Copy-Item $ExePath $DeployDir

$windeployqt = Join-Path $QtDir "bin\windeployqt.exe"
if (-not (Test-Path $windeployqt)) {
    Write-Error "windeployqt not found at $windeployqt"
    exit 1
}

& $windeployqt `
    --release `
    --no-translations `
    --no-system-d3d-compiler `
    --no-opengl-sw `
    (Join-Path $DeployDir "$AppName.exe")

if ($LASTEXITCODE -ne 0) { Write-Error "windeployqt failed"; exit 1 }

# ── 4. Copy translations ──────────────────────────────────────────────────────
Write-Host "[4/4] Copying translations..." -ForegroundColor Yellow
$TransDir = Join-Path $DeployDir "translations"
New-Item -ItemType Directory -Force $TransDir | Out-Null

foreach ($qm in @("ru_RU.qm", "en_US.qm")) {
    $src = Join-Path $BuildDir $qm
    if (Test-Path $src) {
        Copy-Item $src $TransDir
        Write-Host "  Copied $qm"
    } else {
        Write-Warning "  $qm not found in build dir (run lrelease or rebuild)"
    }
}

# ── Done ─────────────────────────────────────────────────────────────────────
$size = (Get-ChildItem $DeployDir -Recurse | Measure-Object -Property Length -Sum).Sum
$sizeMb = [math]::Round($size / 1MB, 1)

Write-Host ""
Write-Host "=== Done! ===" -ForegroundColor Green
Write-Host "Deploy folder: $DeployDir  ($sizeMb MB)"
Write-Host "Run:  .\deploy\$AppName.exe"
Write-Host "      .\deploy\$AppName.exe --light"
