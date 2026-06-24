#Requires -Version 5.1
<#
.SYNOPSIS
    Builds a static Qt 6.8.0 from the already-downloaded sources at C:\Qt\6.8.0\Src.
    Run ONCE. Takes ~60 min. Result: C:\Qt\6.8.0-static\mingw_64

.DESCRIPTION
    Uses the Qt source tree already present on disk (installed by Qt Maintenance Tool).
    Configures with -static -release, skips unused modules, builds with Ninja + MinGW.
    After this, run:  .\deploy.ps1   =>  deploy\AKIP-manager_module.exe + CH375DLL64.dll

.PARAMETER MingwDir
    Path to MinGW 13 bin. Default: C:\Qt\Tools\mingw1310_64\bin

.PARAMETER InstallDir
    Install prefix for static Qt. Default: C:\Qt\6.8.0-static\mingw_64

.PARAMETER QtSrc
    Qt source root. Default: C:\Qt\6.8.0\Src  (already on disk from Qt installer)

.EXAMPLE
    .\build_qt_static.ps1
#>
param(
    [string]$MingwDir  = "C:\Qt\Tools\mingw1310_64\bin",
    [string]$InstallDir = "C:\Qt\6.8.0-static\mingw_64",
    [string]$QtSrc     = "C:\Qt\6.8.0\Src"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$BuildDir = "C:\Qt\6.8.0-static-build"

Write-Host ""
Write-Host "=== Qt 6.8.0 static build ===" -ForegroundColor Cyan
Write-Host "Sources: $QtSrc"
Write-Host "Build:   $BuildDir"
Write-Host "Install: $InstallDir"
Write-Host "MinGW:   $MingwDir"
Write-Host ""

if (-not (Test-Path "$QtSrc\CMakeLists.txt")) {
    Write-Error "Qt sources not found at $QtSrc`nInstall 'Sources' component via Qt Maintenance Tool."
    exit 1
}

foreach ($tool in @("cmake", "ninja", "python", "perl")) {
    if (-not (Get-Command $tool -ErrorAction SilentlyContinue)) {
        Write-Error "$tool not found in PATH.`nInstall it and add to PATH before running this script."
    }
}

$env:PATH = "$MingwDir;C:\Qt\Tools\Ninja;C:\Qt\Tools\CMake_64\bin;" + $env:PATH

# ── Configure ────────────────────────────────────────────────────────────────
Write-Host "[1/3] Configuring static Qt..." -ForegroundColor Yellow
New-Item -ItemType Directory -Force $BuildDir | Out-Null
Push-Location $BuildDir

& "$QtSrc\configure" `
    -prefix "$InstallDir" `
    -release `
    -static `
    -static-runtime `
    -platform win32-g++ `
    -qt-zlib -qt-libpng -qt-libjpeg -qt-freetype `
    -nomake examples `
    -nomake tests `
    -no-opengl `
    -skip qtwebengine `
    -skip qtmultimedia `
    -skip qt3d `
    -skip qtquick3d `
    -skip qtdatavis3d `
    -skip qtvirtualkeyboard `
    -skip qtcharts `
    -skip qtspeech `
    -skip qtlocation `
    -skip qtpositioning `
    -skip qtsensors `
    -skip qtserialbus `
    -skip qtserialport `
    -skip qtbluetooth `
    -skip qtnfc `
    -skip qtremoteobjects `
    -skip qtscxml `
    -skip qtwebsockets `
    -skip qtwebchannel `
    -skip qtwebview `
    -skip qthttpserver `
    -skip qtgrpc `
    -skip qtcoap `
    -skip qtmqtt `
    -skip qtopcua `
    -confirm-license `
    -opensource

if ($LASTEXITCODE -ne 0) { Pop-Location; Write-Error "configure failed"; exit 1 }
Pop-Location

# ── Build ────────────────────────────────────────────────────────────────────
Write-Host "[2/3] Building (~60 min)..." -ForegroundColor Yellow
cmake --build $BuildDir --parallel
if ($LASTEXITCODE -ne 0) { Write-Error "Build failed"; exit 1 }

# ── Install ──────────────────────────────────────────────────────────────────
Write-Host "[3/3] Installing to $InstallDir..." -ForegroundColor Yellow
cmake --install $BuildDir
if ($LASTEXITCODE -ne 0) { Write-Error "Install failed"; exit 1 }

Write-Host ""
Write-Host "=== Done! Static Qt at: $InstallDir ===" -ForegroundColor Green
Write-Host ""
Write-Host "Next step — build portable app:"
Write-Host "  .\deploy.ps1"
Write-Host ""
Write-Host "Result:  deploy\AKIP-manager_module.exe  (standalone, no Qt DLLs)"
Write-Host "         deploy\CH375DLL64.dll            (USB driver)"
