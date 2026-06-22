# AI Context — AKIP/Gratten Generator Manager

## Проект
Standalone Qt/C++ приложение для управления генераторами сигналов АКИП-3417 и Gratten GA1483.
Бывший модуль, превращаемый в самостоятельный продукт.

## Стек
- **Язык:** C++17
- **Фреймворк:** Qt 5/6 (Core, Widgets, Network)
- **Сборка:** CMake 3.16+
- **Платформа:** Windows (АКИП USB требует CH375DLL64.dll)
- **Протокол:** SCPI (Standard Commands for Programmable Instruments)

## Устройства
| Устройство | Подключение | Транспорт | Канал |
|---|---|---|---|
| АКИП-3417 | USB (CH375) | `UsbInterface` | A, B |
| Gratten GA1483 | LAN TCP:5025 | `LanInterface` | 1 (RF) |

## Архитектура (текущая)
```
IAkipController (интерфейс, QObject)
├── AkipFacade          → UsbInterface → CH375DLL64.dll
└── AbstractScpiController
    └── GrattenGa1483Controller → LanInterface → QTcpSocket
```
`MainWindow` владеет одним `IAkipController*`, переключает страницы через `QStackedWidget`.

## Известные баги (legacy)
1. `mainwindow.cpp:299` — `on_btnSetOutputB_clicked` читает `chkOutputA` вместо `chkOutputB`
2. `mainwindow.cpp:284` — `on_btnQueryFreqB_clicked` пишет в `editFreqA` вместо `editFreqB`
3. `mainwindow.cpp:334,339` — `on_btnSetAmplB_clicked` читает поля канала A вместо B
4. `mainwindow.cpp:377` — `on_btnSetWaveB_clicked` читает `cmbWaveformA` вместо `cmbWaveformB`
5. `mainwindow.cpp:9` — IP-адрес Gratten захардкожен: `"192.168.1.66"`
6. `akipfacade.cpp:353` — `setAMState` — заглушка, команда не отправляется

## Задачи для standalone-продукта
- [ ] Убрать все хардкоды (IP, порт) → QSettings
- [ ] Добавить i18n (Qt Linguist, ru/en)
- [ ] Исправить баги channel B
- [ ] Диалог настроек подключения
- [ ] Кросс-платформенность (или минимум — ясные ifdef)
- [ ] Версионирование (version.h)
- [ ] Installer (NSIS или Qt IFW)
- [ ] Unit-тесты на контроллеры

## Решения по архитектуре
- Оставить `IAkipController` как публичный интерфейс
- `AbstractScpiController` — общая база для всех SCPI-устройств
- Добавить `SettingsManager` (QSettings wrapper) для персистентных настроек
- i18n через `QTranslator` + `.ts` файлы, переключение в runtime

## Файлы проекта
| Файл | Роль |
|---|---|
| `iakipcontroller.h` | Публичный интерфейс (не менять API без причины) |
| `abstractscpicontroller.h/cpp` | SCPI-база с кешем, верификацией команд |
| `akipfacade.h/cpp` | АКИП-3417 через USB/CH375 |
| `grattenga1483controller.h/cpp` | Gratten GA1483 через LAN |
| `grattencontrolwidget.h/cpp/.ui` | Виджет управления Gratten |
| `mainwindow.h/cpp/.ui` | Главное окно |
| `laninterface.h/cpp` | TCP-транспорт (SCPI over LAN) |
| `usbinterface.h/cpp` | USB-транспорт (CH375) |
| `gratten_limits.h` | Константы/ограничения Gratten (inline QVector) |
| `ch375_sdk/` | SDK CH375 (только Windows, .dll + .lib + .h) |
