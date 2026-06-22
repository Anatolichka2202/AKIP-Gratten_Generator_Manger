# AKIP/Gratten Generator Manager

Standalone desktop application for controlling AKIP-3417 and Gratten GA1483 signal generators.

> **Автономное десктопное приложение для управления генераторами сигналов АКИП-3417 и Gratten GA1483.**

---

## Поддерживаемые устройства / Supported Devices

| Device | Connection | Protocol |
|---|---|---|
| АКИП-3417 (AKIP-3417) | USB (CH375) | SCPI over USB |
| Gratten GA1483 | LAN (TCP:5025) | SCPI over TCP |

## Требования / Requirements

- Windows 10/11 (64-bit)
- Qt 5.15 or Qt 6.x
- CMake 3.16+
- MinGW 64-bit or MSVC 2019+
- CH375DLL64.dll (included in `ch375_sdk/`)

## Сборка / Build

```bash
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

## Архитектура / Architecture

```
IAkipController          ← public interface
├── AkipFacade           ← AKIP-3417 via USB/CH375
└── AbstractScpiController
    └── GrattenGa1483Controller  ← Gratten GA1483 via LAN
```

Транспортный слой / Transport layer:
- `UsbInterface` — CH375 USB driver wrapper (Windows only)
- `LanInterface` — QTcpSocket SCPI-over-TCP

## Возможности / Features

- Автоопределение подключённых устройств
- Управление частотой, амплитудой, формой сигнала, выходом
- AM-модуляция (частота, глубина, вкл/выкл)
- Скважность (duty cycle) для меандра
- Лог команд с временными метками
- Тест задержек (мин/макс/среднее по серии запросов)

## Структура проекта / Project Structure

```
AKIP-Gratten_Generator_Manger/
├── .claude/          # AI context & tasks (не включается в релиз)
├── ch375_sdk/        # CH375 Windows USB SDK
├── design/           # Design mockups & assets
├── docs/             # Technical documentation
├── education/        # Learning notes & mentor materials
├── *.h / *.cpp       # Source code
├── *.ui              # Qt Designer UI files
└── CMakeLists.txt
```

## Статус / Status

> **В разработке.** Идёт рефакторинг из модуля в самостоятельный продукт.

## Лицензия / License

Proprietary. All rights reserved.
