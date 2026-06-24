# AKIP/Gratten Generator Manager

Standalone Qt 6 desktop application for controlling AKIP-3417 and Gratten GA1483 signal generators via SCPI protocol.

**EN:** Portable Windows EXE (42.8 MB, static Qt build, zero external dependencies except CH375DLL64.dll).
**RU:** Автономное приложение для управления генераторами АКИП-3417 и Gratten GA1483 с поддержкой русского и английского языков.

---

## Поддерживаемые устройства / Supported Devices

| Device | Connection | Protocol | Channels |
|---|---|---|---|
| АКИП-3417 (AKIP-3417) | USB (CH375) | SCPI | 2 (A, B) |
| Gratten GA1483 | LAN (TCP:5025) | SCPI | 1 (RF) |

## Быстрый старт / Quick Start

### Развёртывание (Deployment)
1. Скачайте готовый build: `deploy/AKIP-manager_module.exe` + `CH375DLL64.dll`
2. Положите в одну папку
3. Запустите EXE — никаких зависимостей, никакой установки

### Разработка (Development)
```bash
# Build динамический (Qt DLLs) для быстрой разработки
cmake --preset release-dynamic
cmake --build build_release --parallel

# Запуск из Qt Creator: Open Projects → выбрать этот проект
```

### Статический деплой (Static Deploy)
```powershell
# Один раз: скомпилировать Qt 6.8 static
.\build_qt_static.ps1

# Сборка portable EXE (42.8 MB)
.\deploy.ps1

# Результат: deploy/AKIP-manager_module.exe
```

## Архитектура / Architecture

```
IAkipController          ← public interface
├── AkipFacade           ← AKIP-3417 via USB/CH375
└── AbstractScpiController
    └── GrattenGa1483Controller  ← Gratten GA1483 via LAN
```

Transport layer:
- `UsbInterface` — CH375 USB driver (Windows only)
- `LanInterface` — QTcpSocket SCPI-over-TCP

## Возможности / Features

- 🌍 Bilingual UI: русский/English с переключением в runtime
- 🔌 Auto-detection: автоматический поиск подключённых устройств
- 📡 Full SCPI support: frequency, amplitude, waveform, output control
- 🎛️ AM/FM modulation (Gratten)
- 📊 Latency diagnostics: min/max/avg over series of queries
- 📝 Command logging with timestamps
- ⚙️ Settings persistence (QSettings)

## Требования / Requirements

**Windows 10/11 (64-bit), никаких зависимостей**
- Приложение: все в один EXE (static Qt 6.8)
- USB для АКИП-3417: CH375DLL64.dll (включен)
- LAN для Gratten: встроенный QTcpSocket

**Для разработки:**
- Qt 6.8.0 MinGW x64
- CMake 3.16+
- Visual Studio Code / Qt Creator

## Структура проекта / Project Structure

```
AKIP-Gratten_Generator_Manger/
├── .claude/           # AI context, tasks, docs
├── ch375_sdk/         # CH375 USB SDK (Windows)
├── design/            # UI mockups
├── docs/              # Architecture, bugs, API
├── education/         # Learning notes
├── tests/             # Unit tests (Qt Test)
├── translations/      # ru_RU.ts + en_US.ts → .qm
├── CMakeLists.txt
├── CMakePresets.json  # debug / release-dynamic / release-static
├── deploy.ps1         # Portable packaging script
└── build_qt_static.ps1 # Static Qt 6.8 build script
```

## Статус / Status

**v0.2.0 — Production Ready**
- ✅ All core features stable
- ✅ Full i18n (ru_RU / en_US)
- ✅ Portable static build
- ✅ Unit tests passing
- ✅ Live language switching

## Лицензия / License

Proprietary. All rights reserved.
