# TASKS — Список задач с микрозадачами

## TASK-01: Исправить setChannelControlsEnabled для Gratten-режима
**Приоритет**: Medium | **Размер**: XS

- [ ] В `setChannelControlsEnabled(bool)`: добавить ветку `if (m_currentType == GRATTEN)`
- [ ] В Gratten-режиме: вызывать `m_grattenPage->setEnabled(enabled)` вместо channelA/B
- [ ] В AKIP-режиме: вызывать `m_channelA/B->setEnabled(enabled)`
- [ ] Проверить что `grpDelays` актуален только для АКИП; скрыть при Gratten

---

## TASK-02: tr() обёртки в mainwindow.cpp
**Приоритет**: High | **Размер**: S

- [ ] Пройти mainwindow.cpp, найти все строковые литералы вне `tr()`
- [ ] Обернуть каждую в `tr("...")`
- [ ] Проверить: `lupdate translations/ru_RU.ts` должен подобрать новые строки
- [ ] Добавить русские переводы в `ru_RU.ts`

---

## TASK-03: GrattenControlWidget — секции FM/PM/Sweep/PULM
**Приоритет**: High | **Размер**: M

- [ ] В `grattencontrolwidget.ui`: добавить `QGroupBox "FM модуляция"` (скрыт по умолчанию)
  - поля: devHz (QLineEdit), freqHz (QLineEdit), ON/OFF (QCheckBox)
  - кнопки Set/Query для каждого
- [ ] Добавить `QGroupBox "PM модуляция"` (скрыт)
  - devRad, freqHz, ON/OFF
- [ ] Добавить `QGroupBox "Sweep"` (скрыт)
  - startHz, stopHz, dwellSec, ON/OFF
- [ ] Добавить `QGroupBox "PULM / импульс"` (скрыт)
  - period, width, ON/OFF
- [ ] Добавить кнопку "Показать расширенные" для toggle всех секций
- [ ] В `grattencontrolwidget.cpp`: подключить слоты к `IAkipController` методам

---

## TASK-04: StatusBar
**Приоритет**: Medium | **Размер**: S

- [ ] В `mainwindow.cpp/h`: добавить `QLabel` × 3 в `statusBar()`
  - lblDevice: "АКИП-3417" / "Gratten GA1483" / "Не подключено"
  - lblConnection: "192.168.1.66:5025" (из SettingsManager) или "USB"
  - lblLastOp: "Последняя команда: 42 мс" (обновлять после каждой операции)
- [ ] `updateStatusBar()` — вызывать из `updateConnectionStatus()` и после команд
- [ ] Убрать дублирование из `ui->lblDeviceType` (либо оставить, либо перенести в статусбар)

---

## TASK-05: ToolBar
**Приоритет**: Low | **Размер**: S

- [ ] Добавить `QToolBar` в `MainWindow`
- [ ] Кнопки: Подключить, Отключить, IDN, Reset, Настройки
- [ ] Иконки: использовать стандартные `QStyle::StandardPixmap` (нет зависимостей)
- [ ] Disable/enable по состоянию подключения

---

## TASK-06: Иконка приложения
**Приоритет**: Low | **Размер**: XS

- [ ] Создать `resources/icon.ico` (16/32/48/256 px)
- [ ] Добавить `resources.qrc` с иконкой
- [ ] В `CMakeLists.txt`: добавить `resources.qrc` в `add_executable`
- [ ] В `main.cpp`: `QApplication::setWindowIcon(QIcon(":/icon"))`
- [ ] На Windows: добавить `.rc` файл для иконки в заголовке окна

---

## TASK-07: Переименовать IAkipController → ISignalGenerator
**Приоритет**: Low | **Размер**: S (чистый рефакторинг)

- [ ] Переименовать файл `iakipcontroller.h` → `isignalgenerator.h`
- [ ] Переименовать класс `IAkipController` → `ISignalGenerator`
- [ ] Обновить все `#include` и использования (mainwindow, facade, controllers)
- [ ] Обновить `CMakeLists.txt`
- [ ] Обновить `ARCHITECTURE.md`

---

## TASK-08: AkipFacade через ITransport
**Приоритет**: Medium | **Размер**: M

- [ ] Добавить конструктор `AkipFacade(ITransport *transport, QObject *parent = nullptr)`
- [ ] Внутри использовать `m_transport->send()` вместо `m_usb.sendScpiCommand()`
- [ ] `UsbTransport` — обёртка `UsbInterface` реализующая `ITransport`
- [ ] Обновить `main.cpp`: создавать `UsbTransport`, передавать в `AkipFacade`
- [ ] Написать unit-тест `test_akip_facade.cpp` с `MockTransport`

---

## TASK-09: Верификация АКИП команд с железом
**Приоритет**: High | **Размер**: S (требует железо)

- [ ] Проверить `AM:STAT ON/OFF` — принимает ли АКИП-3417 эту команду
- [ ] Проверить `FM:STAT ON/OFF` — аналогично
- [ ] Проверить `BURS:CHA:INT:PER` и `BURS:CHA:STAT` (burst mode)
- [ ] Задокументировать результаты в `docs/DEVICES_COMMANDS.md`
- [ ] Обновить `AkipFacade` согласно результатам (убрать TODO-комменты)

---

## TASK-10: QSS тема
**Приоритет**: Low | **Размер**: M

- [ ] Создать `design/style/light.qss`
- [ ] Создать `design/style/dark.qss`
- [ ] В `SettingsManager`: добавить `theme()` / `setTheme()`
- [ ] В `main.cpp`: загружать QSS из ресурсов
- [ ] В `SettingsDialog`: добавить переключатель темы

---

## TASK-11: Дополнить unit-тесты
**Приоритет**: Medium | **Размер**: M

- [ ] `tests/test_gratten_controller.cpp`: тесты GrattenGa1483Controller с MockTransport
  - setFrequency → проверить SCPI строку
  - setAMState → проверить `:AM:STATE ON`
  - setSweepStart → проверить `:SWEep:STARt`
- [ ] `tests/test_akip_facade.cpp`: тесты AkipFacade с MockTransport (требует TASK-08)
  - setFrequency ch1 → `FREQ:CHA 1000`
  - setOutput ch2 false → `OUTP:CHB OFF`
- [ ] Добавить тесты в `tests/CMakeLists.txt`
- [ ] Убедиться что CI запускает все тесты

---

## TASK-12: Настроить Windows CI
**Приоритет**: Medium | **Размер**: M

- [ ] В `.github/workflows/build.yml`: добавить job `build-windows`
  - `runs-on: windows-latest`
  - `jurplel/install-qt-action` с Qt 6.x MinGW
  - cmake с `-DCMAKE_PREFIX_PATH` для Qt
  - Без CH375 SDK: добавить stub-либо или флаг `-DNO_USB_INTERFACE=ON` явно
- [ ] Проверить что сборка проходит без CH375 SDK
- [ ] Документировать в README как собрать с CH375 SDK вручную
