# AI Context — AKIP/Gratten Generator Manager

> Обновлено: 2026-06-24  |  Версия: 0.2.0  |  Статус: Alpha → Production Ready

## Проект
Standalone Qt/C++ приложение для управления генераторами сигналов АКИП-3417 и Gratten GA1483.

## Стек
- **Язык:** C++17
- **Фреймворк:** Qt 6.8.0 (Core, Widgets, Network)
- **Сборка:** CMake 3.16+, Ninja, MinGW 13.1 x64
- **Платформа:** Windows (USB требует CH375DLL64.dll)
- **Протокол:** SCPI (Standard Commands for Programmable Instruments)
- **Деплой:** Portable EXE (43 МБ, статическая линковка Qt), без инсталлятора

## Устройства
| Устройство     | Подключение | Транспорт      | Каналы |
|----------------|-------------|----------------|--------|
| АКИП-3417      | USB CH375   | `UsbInterface` | A, B   |
| Gratten GA1483 | LAN TCP:5025| `LanInterface` | 1 (RF) |

## Архитектура (текущая)
```
IAkipController (интерфейс, QObject)
├── AkipFacade          → UsbInterface → CH375DLL64.dll
└── AbstractScpiController
    └── GrattenGa1483Controller → LanInterface → QTcpSocket

QMainWindow
└── MainWindow
    ├── QStackedWidget
    │   ├── akipPage (ChannelWidget A + B)
    │   ├── grattenPage (GrattenControlWidget)
    │   └── splashPage (SplashWidget — нет устройства)
    ├── LanguageSwitcher (ru_RU / en_US, runtime)
    └── StatusBar (тип, адрес, последняя операция)
```

## Что уже сделано
- Все баги legacy-кода (BUG-001..007) исправлены
- SettingsManager — QSettings, нет хардкодов IP/порта
- i18n: tr() везде, ru_RU + en_US, live switching через changeEvent
- Статическая Qt 6.8 собрана из Src/ → C:\Qt\6.8.0-static\mingw_64
- Portable EXE: AKIP-manager_module.exe (43 МБ) + CH375DLL64.dll
- DiagnosticsDialog: тесты задержек IDN/Freq (вынесены из MainWindow)
- Иконка (ICO), app.rc, WindowIcon
- qDebug заглушён в Release (#ifndef QT_NO_DEBUG)
- Unit-тесты: GrattenController (4 теста), SettingsManager (5 тестов), MockTransport

## Известные ограничения
- setAMState для АКИП-3417 — заглушка (AM:STAT синтаксис не верифицирован с железом)
- FM/sweep/burst для АКИП-3417 — не реализованы (нужна документация)
- Только Windows (CH375DLL)

## Файлы проекта
| Файл | Роль |
|---|---|
| `iakipcontroller.h` | Публичный интерфейс (не менять API) |
| `abstractscpicontroller.h/cpp` | SCPI-база с кешем |
| `akipfacade.h/cpp` | АКИП-3417 через USB/CH375 |
| `grattenga1483controller.h/cpp` | Gratten GA1483 через LAN |
| `grattencontrolwidget.h/cpp/.ui` | Виджет управления Gratten |
| `channelwidget.h/cpp/.ui` | Виджет одного канала (A/B) |
| `mainwindow.h/cpp/.ui` | Главное окно |
| `splashwidget.h/cpp` | Экран без устройства |
| `diagnosticsdialog.h/cpp` | Диагностика: задержки IDN/Freq |
| `languageswitcher.h/cpp` | Смена языка в runtime |
| `settingsmanager.h/cpp` | QSettings wrapper |
| `settingsdialog.h/cpp/.ui` | Диалог настроек подключения |
| `laninterface.h/cpp` | TCP-транспорт |
| `usbinterface.h/cpp` | USB-транспорт (CH375) |
| `mocktransport.h/cpp` | Mock для unit-тестов |
| `translations/ru_RU.ts` | Русский (source) |
| `translations/en_US.ts` | Английский |
| `build_qt_static.ps1` | Сборка Qt 6.8 из Src/ (~60 мин, один раз) |
| `deploy.ps1` | Сборка + упаковка portable (EXE + DLL) |
| `CMakePresets.json` | release-static / release-dynamic / debug |
| `ch375_sdk/` | SDK CH375 (.dll + .lib + .h, только Windows) |
| `tests/` | Qt Test: граттен, настройки, транспорт |
