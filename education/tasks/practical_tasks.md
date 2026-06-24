# Практические задачи — AKIP/Gratten Generator Manager

> Привязка к реальному коду проекта. Каждая задача — конкретное улучшение или расширение текущей кодовой базы.

---

## A. Backend / Логика / Железо

### A-1. Реализация FM/Sweep/Burst для АКИП-3417
**Сложность:** Medium

В `akipfacade.cpp` есть только заглушка для AM. Задача: изучить SCPI-документацию АКИП-3417 (аналог Rigol DG1022), реализовать `setFmState`, `setSweepState`, `setBurstState`. Протестировать через `MockTransport` — проверить правильность команд (`FM:STAT ON`, `SWE:STAT ON`, `BURS:STAT ON`). Добавить кнопки в `ChannelWidget`.

**Навыки:** SCPI-протокол, документация приборов, расширение фасада, TDD с MockTransport
**Готово когда:** Три режима формируют корректные SCPI-команды (покрыто тестами), кнопки в `ChannelWidget`, регрессий нет.

---

### A-2. Cross-platform USB через libusb
**Сложность:** Hard

`UsbInterface` завязан на `CH375DLL64.dll` — только Windows. Создать `LibUsbInterface`, реализующий тот же `ITransport` через libusb-1.0. Выбор транспорта через CMake опцию `USE_LIBUSB`. В `SettingsManager` добавить поле выбора. Открывает путь к Linux/macOS.

**Навыки:** USB-библиотеки, паттерн Strategy, условная компиляция CMake, кроссплатформенный C++
**Готово когда:** Проект собирается с `-DUSE_LIBUSB=ON` на Linux без CH375DLL, базовые тесты проходят.

---

### A-3. Кэширование и батчинг SCPI-команд
**Сложность:** Medium

`AbstractScpiController` уже имеет кэш `m_cache`. Расширить до батч-механизма — накапливать серию команд и отправлять блоком через разделитель `;`. Реализовать `ScpiCommandBatch` с API `add(cmd)` / `commit()`. Измерить разницу через `DiagnosticsDialog`.

**Навыки:** Паттерн Command + Batch, оптимизация I/O, бенчмаркинг
**Готово когда:** Unit-тест подтверждает, что 3 команды уходят одной строкой; DiagnosticsDialog показывает улучшение.

---

### A-4. Одновременное подключение двух устройств
**Сложность:** Hard

Сейчас `MainWindow` показывает либо АКИП, либо Gratten. `DeviceManager` как `QMap<DeviceType, IAkipController*>`. `QSplitter` вместо `QStackedWidget` — оба виджета одновременно. `SplashWidget` показывается только для своего устройства.

**Навыки:** Управление жизненным циклом Qt-объектов, паттерн Registry, рефакторинг MainWindow
**Готово когда:** Оба устройства подключены и управляются независимо.

---

### A-5. Запись и воспроизведение последовательностей команд
**Сложность:** Hard

`CommandSequence` хранит список `{timestamp_ms, scpi_command}`, сериализуется в JSON. `SequenceRecorder` перехватывает вызовы через `IAkipController` (паттерн Decorator). `SequencePlayer` воспроизводит с оригинальными задержками. UI: `SequenceDialog`.

**Навыки:** Паттерн Decorator, JSON в Qt, `QTimer`, проектирование диалогов
**Готово когда:** 5+ команд записываются, сохраняются в `.seq`, воспроизводятся с правильными задержками.

---

## B. Frontend / UI / Qt

### B-1. Виджет осциллограммы в реальном времени
**Сложность:** Medium

`WaveformWidget : QWidget` с отрисовкой формы сигнала через `QPainter` + `QPainterPath`. Анимация 30 FPS через `QTimer`. Поддержка форм: синус, прямоугольник, треугольник, пила. Интеграция в `GrattenControlWidget`.

**Навыки:** Кастомные QWidget, QPainter, QPainterPath, математика сигналов
**Готово когда:** Форма корректна при изменении параметров, анимация плавная, CPU idle < 5%.

---

### B-2. Переключение тем в runtime
**Сложность:** Easy

`ThemeManager` с темами Catppuccin уже есть. Добавить сигнал `themeChanged(Theme)`, подписку всех виджетов, тему "System" (определяет тёмность Windows через `QStyleHints`). Сохранять выбор в `SettingsManager`.

**Навыки:** `QPalette`, `QApplication::setPalette()`, Observer через Qt signals
**Готово когда:** Тема меняется без перезапуска, системная тема определяется автоматически.

---

### B-3. Toolbar с горячими клавишами
**Сложность:** Easy

Закрыть backlog-пункт из `tasks.md`. `QToolBar` в `MainWindow`: подключить/отключить, настройки, тема, вкл/выкл канал. `QKeySequence` для каждого действия. Иконки из Qt Standard Icons или SVG в `resources.qrc`.

**Навыки:** `QToolBar`, `QAction`, `QKeySequence`, shared QAction между тулбаром и меню
**Готово когда:** Тулбар работает, горячие клавиши документированы, строки переводятся при смене языка.

---

### B-4. График истории параметров
**Сложность:** Medium

`HistoryPlotWidget` на `QChartView` или QPainter. Каждые N секунд опрашивает `queryFrequency()`, скользящее окно 300 точек. Экспорт в CSV через `QFile`. Интеграция в `DiagnosticsDialog`.

**Навыки:** Qt Charts, таймеры опроса, экспорт CSV
**Готово когда:** 5 минут истории, экспорт работает, интервал опроса настраивается.

---

### B-5. Drag-and-drop пресеты параметров
**Сложность:** Medium

`PresetPanel` (QDockWidget) со списком пресетов `{name, frequency, amplitude, waveform}`. Перетащить на `ChannelWidget` — параметры применяются. Сериализация в JSON. Import/export пресетов в файл.

**Навыки:** `QDockWidget`, `QDrag`/`QDrop`, `QMimeData`, `QJsonDocument`
**Готово когда:** Drag-and-drop работает, import/export между запусками.

---

## C. Инфраструктура / DevOps

### C-1. CI/CD через GitHub Actions
**Сложность:** Medium

`.github/workflows/build.yml`: checkout → MinGW → кэш Qt → `cmake --preset release-static` → `ctest`. Badge в README. Тесты используют `MockTransport` (CH375DLL не нужен в CI).

**Навыки:** GitHub Actions YAML, кэш CI, CTest, Windows runner
**Готово когда:** Pipeline зелёный на main, все тесты проходят, badge в README.

---

### C-2. NSIS-инсталлятор с проверкой обновлений
**Сложность:** Medium

`installer/installer.nsi`: директория установки, ярлыки, запись в "Программы и компоненты", деинсталлятор. `UpdateChecker` через `QNetworkAccessManager` запрашивает GitHub Releases API, сравнивает с `APP_VERSION_STRING`.

**Навыки:** NSIS scripting, Windows registry, `QNetworkAccessManager`, `QJsonDocument`
**Готово когда:** Установщик ставит/удаляет корректно, проверка обновлений работает.

---

### C-3. Doxygen документация + GitHub Pages
**Сложность:** Easy

Doxygen-комментарии к `iakipcontroller.h`, `abstractscpicontroller.h`, `settingsmanager.h`. `Doxyfile` с HTML + Graphviz. GitHub Actions публикует на GitHub Pages.

**Навыки:** Doxygen (`@brief`, `@param`, `@return`), Graphviz, GitHub Pages
**Готово когда:** `doxygen Doxyfile` без warnings, диаграмма классов правильная, Pages публикуется автоматически.

---

## D. Наука и инженерия

### D-1. Анализ спектра сигнала (FFT)
**Сложность:** Hard

`SpectrumAnalyzerWidget` вычисляет спектр по параметрам генератора. DFT или FFTW3. Логарифмическая ось Y (дБ), линейная X (Гц). Маркеры пиков с аннотациями.

**Навыки:** ДПФ, `std::complex<double>`, оконные функции (Hann/Hamming), логарифмические шкалы
**Готово когда:** Пик на нужной частоте виден, гармоники прямоугольника отображаются.

---

### D-2. Калибровка и адаптивные таймауты
**Сложность:** Medium

`DiagnosticsDialog` уже измеряет задержки. Расширить: 100 замеров, вычислить mean/median/stddev/p95. Сохранить в `calibration.json`. `AbstractScpiController` использует калибровочные данные для адаптивного таймаута.

**Навыки:** Описательная статистика (`std::sort`, `std::accumulate`), адаптивные таймауты, JSON
**Готово когда:** Статистика показывается; адаптивный таймаут снижает число ошибок при нестабильном соединении.

---

### D-3. Автосканирование SCPI-устройств в сети
**Сложность:** Medium

`NetworkScanner` сканирует /24 подсеть по порту 5025, отправляет `*IDN?` каждому хосту. `QtConcurrent::mapped` для параллельного сканирования. `DeviceDiscoveryDialog` — список найденных устройств.

**Навыки:** Асинхронный `QTcpSocket`, `QtConcurrent`, CIDR-генерация IP
**Готово когда:** Сканирование /24 < 10 сек, Gratten отображается с именем из IDN, двойной клик подключает.
