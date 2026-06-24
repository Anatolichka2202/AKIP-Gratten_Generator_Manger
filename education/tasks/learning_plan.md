# 12-недельный план обучения — AKIP/Gratten Generator Manager

> Каждая неделя: тема + задача из practical_tasks.md + ожидаемый результат.

---

## Блок 1 — C++17 + Qt Core (Недели 1–3)

### Неделя 1 — Современный C++17
**Темы:** `std::optional`, `std::variant`, structured bindings, `[[nodiscard]]`, move semantics в контексте Qt

**Изучи в проекте:**
- `abstractscpicontroller.h` — как используется `QMap<QString, double> m_cache`
- `settingsmanager.cpp` — паттерн "обёртка над QSettings"

**Задача:** Добавить `std::optional<double>` как возвращаемый тип `queryFrequency()` вместо `-1.0` как маркера ошибки. Обновить всех вызывателей.

**Ресурсы:** cppreference.com, "A Tour of C++" (Stroustrup) гл. 5–8

**Результат:** Понимаешь, где legacy C++11 в проекте и как улучшить без поломки API.

---

### Неделя 2 — Qt Signals/Slots и метаобъектная система
**Темы:** `Q_OBJECT`, moc, синтаксисы connect, `QObject` ownership, лямбды в connect

**Изучи в проекте:**
- `languageswitcher.cpp` — смена языка через сигналы
- `mainwindow.cpp` — SplashWidget ↔ QStackedWidget

**Задача:** Реализовать задачу **B-2** (переключение тем). Сигнал `themeChanged`, подписка виджетов.

**Ресурсы:** Qt Documentation → "Signals and Slots"

**Результат:** Умеешь трассировать поток сигналов, понимаешь утечки через лямбды.

---

### Неделя 3 — Qt Widgets и кастомные компоненты
**Темы:** `QWidget::paintEvent`, `QPainter`, `QPainterPath`, `QTimer` + `update()`, layout system

**Изучи в проекте:**
- `channelwidget.ui` + `channelwidget.cpp` — как .ui превращается в код
- `splashwidget.cpp` — кастомный виджет без .ui

**Задача:** Реализовать задачу **B-1** (WaveformWidget). Синус → прямоугольник → треугольник.

**Ресурсы:** Qt Examples → "Analog Clock", "Basic Drawing"

**Результат:** Собственный QWidget с анимацией, понимание paint pipeline.

---

## Блок 2 — Архитектурные паттерны (Недели 4–6)

### Неделя 4 — Паттерн Interface + Factory
**Темы:** Pure virtual в C++, Factory Method, Dependency Injection

**Изучи в проекте:**
- `iakipcontroller.h` — каждый метод и его контракт
- `mainwindow.cpp` — как AkipFacade и GrattenGa1483Controller создаются и подменяются

**Задача:** Начать реализацию **A-4** (DeviceManager, multi-device). QSplitter с двумя панелями.

**Ресурсы:** "Design Patterns" GoF — Factory, "C++ Software Design" (Klaus Iglberger)

**Результат:** Понимаешь, почему `IAkipController*`, а не `AkipFacade*` передаётся в виджеты.

---

### Неделя 5 — Паттерны State и Command
**Темы:** Конечный автомат для подключения устройства, Command как объект, Undo/Redo

**Изучи в проекте:**
- `splashwidget.cpp` — неявный State (подключено/нет)
- `abstractscpicontroller.cpp` — как команды отправляются без инкапсуляции

**Задача:** Реализовать **A-3** (`ScpiCommandBatch`). Набросок `QStateMachine` для DeviceConnection.

**Ресурсы:** Qt Documentation → `QStateMachine`, "Head First Design Patterns"

**Результат:** `ScpiCommandBatch` работает и покрыт тестами.

---

### Неделя 6 — Паттерны Decorator и Observer
**Темы:** Decorator над `IAkipController`, Observer через Qt signals, JSON сериализация

**Изучи в проекте:**
- `mocktransport.cpp` — Stub над реальным транспортом
- `settingsmanager.cpp` — сериализация через QSettings

**Задача:** Реализовать **A-5** (`SequenceRecorder` + `SequencePlayer`). Классический Decorator.

**Ресурсы:** refactoring.guru/ru — Decorator, Qt Documentation → QJsonDocument

**Результат:** `SequenceRecorder` перехватывает команды прозрачно, `.seq` файл воспроизводится.

---

## Блок 3 — Embedded / Железо (Недели 7–9)

### Неделя 7 — SCPI-протокол и инструментальные интерфейсы
**Темы:** SCPI IEEE 488.2, структура команд, сравнение АКИП-3417 vs Gratten GA1483, обработка ошибок

**Изучи в проекте:**
- `grattenga1483controller.cpp` — полная реализация SCPI через LAN
- `akipfacade.cpp` — та же логика через USB

**Задача:** Реализовать **A-1** (FM/Sweep/Burst для АКИП-3417). TDD: сначала тест, потом реализация.

**Ресурсы:** SCPI-99 Standard (PDF), Rigol DG1022 Programmer Manual

**Результат:** 3 режима с unit-тестами, понимаешь структуру SCPI для любого прибора.

---

### Неделя 8 — TCP/IP и сетевое программирование в Qt
**Темы:** `QTcpSocket` sync vs async, `waitForReadyRead()` vs `readyRead()`, timeout handling, SCPI framing

**Изучи в проекте:**
- `laninterface.cpp` — синхронный LAN-транспорт с таймаутом
- `diagnosticsdialog.cpp` — измерение RTT

**Задача:** Реализовать **D-3** (`NetworkScanner`). `QtConcurrent::mapped` для параллельного сканирования.

**Ресурсы:** Qt Documentation → QTcpSocket, Qt Network Examples → "Fortune Client"

**Результат:** Сканирование /24 < 10 сек, `DeviceDiscoveryDialog` работает.

---

### Неделя 9 — USB и нативные интерфейсы
**Темы:** USB-стек, CH375 DLL API, libusb-1.0 API, условная компиляция в CMake

**Изучи в проекте:**
- `usbinterface.cpp` — обёртка над CH375DLL
- `ch375_sdk/CH375DLL.h` — API нативной DLL
- `CMakeLists.txt` — как CH375DLL подключается

**Задача:** Начать **A-2** (`LibUsbInterface`). Заглушка с теми же методами, CMake опция `USE_LIBUSB`.

**Ресурсы:** libusb.info, "USB in a NutShell" (beyondlogic.org)

**Результат:** `LibUsbInterface` компилируется, USB-дескрипторы АКИП-3417 определены.

---

## Блок 4 — Продуктовый уровень (Недели 10–12)

### Неделя 10 — Тестирование и TDD
**Темы:** Qt Test, `QCOMPARE`/`QVERIFY`/`QSignalSpy`, Test doubles (Mock vs Stub vs Fake), gcov + lcov, FIRST

**Изучи в проекте:**
- `tests/test_gratten_controller.cpp` — образцовые тесты
- `tests/test_settings_manager.cpp` — тесты roundtrip
- `tests/mock_transport.cpp` — как Mock работает без железа

**Задача:** Написать `test_akip_facade` (незакрытый пункт в tasks.md). Покрыть `setFrequency`, `setAmplitude`, `setWaveform`, `setOutputState` для каналов A и B.

**Ресурсы:** Qt Documentation → Qt Test, "xUnit Test Patterns" (Meszaros)

**Результат:** 8+ тестов, все тесты зелёные, понимание MockTransport.

---

### Неделя 11 — CI/CD и автоматизация
**Темы:** GitHub Actions (jobs, steps, cache, artifacts), CMake Presets, CTest, clang-tidy

**Изучи в проекте:**
- `CMakePresets.json` — три пресета
- `deploy.ps1` — текущий ручной деплой
- `build_qt_static.ps1` — что кэшировать в CI

**Задача:** Реализовать **C-1** (GitHub Actions pipeline). Зелёный badge на main.

**Ресурсы:** docs.github.com/actions, CMake Documentation → CTest

**Результат:** Автоматический CI при каждом push, badge в README.

---

### Неделя 12 — Деплой, документация, финальный polish
**Темы:** Статическая линковка Qt, NSIS vs portable, Doxygen + Graphviz, semantic versioning

**Изучи в проекте:**
- `version.h` — `APP_VERSION_STRING`, `APP_NAME`
- `deploy.ps1` — деплой-пайплайн
- `translations/` — как `.ts` → `.qm` в CMake

**Задача:** Реализовать **C-3** (Doxygen + GitHub Pages). Git-тег `v0.2.0`, `CHANGELOG.md`.

**Ресурсы:** doxygen.nl/manual, keepachangelog.com

**Результат:** Живая документация на GitHub Pages, git-тег, проект готов к v0.3.0.

---

## Итог 12 недель

| Блок | Навык | Задачи |
|------|-------|--------|
| 1–3  | C++17 + Qt Widgets | B-1, B-2 |
| 4–6  | Паттерны проектирования | A-3, A-4, A-5 |
| 7–9  | Протоколы и железо | A-1, A-2 старт, D-3 |
| 10–12 | Качество и деплой | test_akip_facade, C-1, C-3 |

**Самостоятельно после плана:** A-2 (libusb завершение), B-3, B-4, B-5, C-2, D-1, D-2.
