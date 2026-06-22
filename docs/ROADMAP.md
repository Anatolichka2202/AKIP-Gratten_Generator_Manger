# Roadmap — AKIP/Gratten Generator Manager

## Видение продукта

Профессиональный инструмент лаборатории/стенда для управления генераторами сигналов с возможностью:
- работы с любым SCPI-совместимым прибором (не только АКИП и Gratten)
- автоматизации последовательностей команд
- интеграции в автоматизированные тестовые системы

---

## Фаза 0 — Workspace ✅
**Статус: Завершена**

- [x] Анализ legacy-кода
- [x] Структура .claude/, docs/, design/, education/
- [x] README.md, ARCHITECTURE.md, BUGS.md
- [x] Первый обучающий урок

---

## Фаза 1 — Стабильное ядро ✅
**Статус: Завершена**

- [x] Исправлено 7 legacy-багов (channel B copy-paste ошибки)
- [x] SettingsManager (QSettings, без хардкода IP)
- [x] SettingsDialog (IP, порт, таймауты Gratten)
- [x] FM-модуляция в IAkipController + реализация для Gratten и АКИП
- [x] Переименован CMake-проект
- [x] Ресерч команд: AKIP=Suin TFG, Gratten=Atten/RIGOL DSG800-совместимый
- [x] Расширен gratten_limits.h (FM, PM, sweep, PULM, LF)

---

## Фаза 2 — Полнота API ✅
**Статус: Завершена (ожидание merge PR #3, #4)**

### 2.1 Граттен — расширенный API
- [x] PM-модуляция (setPMState, setPMDeviation, setPMFrequency) — PR #3
- [x] Sweep (setSweepStart, setSweepStop, setSweepDwellTime, setSweepState) — PR #3
- [x] Pulse modulation (setPULMState, setPULMWidth, setPULMPeriod) — PR #3
- [x] LF output (setLFState, setLFFrequency, setLFAmplitude) — PR #3
- [x] Display update toggle (DISPUPD в gratten_limits.h)
- [ ] AM source selection (setAMSource: INT/EXT) — отложено
- [ ] Reference oscillator (setRefSource: INT/EXT) — отложено

### 2.2 АКИП-3417 — уточнение команд
- [ ] Проверить AM:STAT синтаксис с физическим прибором
- [ ] Проверить FM:STAT синтаксис с физическим прибором
- [x] DC offset для канала B (VOLT:CHB:OFFS) — в availableCommands
- [x] Sweep (SWE:CHB:STAR/STOP/TIME/STAT) — стаб PR #3
- [x] Burst mode (BURS:CHB:INT:PER/STAT) — стаб PR #3
- [x] Ramp symmetry (FUNC:CHB:RAMP:SYMM) — в availableCommands

### 2.3 version.h
- [x] Создать version.h с макросами APP_VERSION, APP_NAME, APP_ORG — PR #4

---

## Фаза 3 — UI/UX 🔄
**Статус: В работе**

### 3.1 Рефакторинг MainWindow
- [x] Вынести методы Channel A/B в отдельный ChannelWidget — PR #6
- [ ] GrattenControlWidget: добавить FM, sweep, PULM секции (скрываемые QGroupBox)
- [ ] StatusBar: тип устройства, IP:порт, время последней операции, счётчик команд
- [ ] ToolBar: быстрые кнопки (Подключить, IDN, Reset, Настройки)

### 3.2 Дизайн
- [ ] Вайрфреймы в design/mockups/
- [ ] QSS-тема (светлая + тёмная) в design/style/
- [ ] Иконка приложения (resources/icon.ico)

### 3.3 Диалоги
- [x] ModulationDialog — AM/FM (PM после merge PR #3) — PR #9
- [x] SweepDialog — настройка свипа — PR #8
- [x] AboutDialog — версия, сборка, лицензия — PR #7

---

## Фаза 4 — i18n (Интернационализация) 🔄
**Статус: Частично**

- [x] GrattenControlWidget: все строки в tr() — PR #5
- [x] ChannelWidget: все строки в tr() — PR #6
- [ ] Обернуть UI-строки в tr() в mainwindow.cpp
- [ ] Создать .ts файлы: `translations/ru_RU.ts`, `translations/en_US.ts`
- [ ] Настроить CMake: lupdate, lrelease
- [ ] LanguageSwitcher в меню (Настройки → Язык)
- [ ] QTranslator: загрузка из QSettings при старте

---

## Фаза 5 — Тесты и качество 🔲
**Статус: Не начата**

- [ ] MockTransport: фиктивный транспорт для unit-тестов без физического прибора
  ```cpp
  class MockTransport : public ITransport {
      QMap<QString, QString> m_responses; // cmd → response
  };
  ```
- [ ] Qt Test unit-тесты для GrattenGa1483Controller (с MockTransport)
- [ ] Qt Test unit-тесты для AkipFacade (с MockTransport)
- [ ] CI: GitHub Actions (.github/workflows/build.yml) для Windows MinGW
- [ ] Doxygen конфигурация для API-документации

---

## Фаза 6 — Автоматизация и скриптинг 🔲
**Статус: Концепт**

- [ ] CommandSequencer: запись/воспроизведение последовательностей команд
  ```
  Формат: JSON или CSV
  { "time_ms": 0,   "cmd": "setFrequency", "args": [1, 1000000] },
  { "time_ms": 100, "cmd": "setOutput",    "args": [1, true] },
  { "time_ms": 5000,"cmd": "setOutput",    "args": [1, false] }
  ```
- [ ] Python/CLI мост: запуск команд из командной строки
- [ ] Экспорт/импорт конфигурации в JSON

---

## Фаза 7 — Кросс-платформенность 🔲
**Статус: Концепт**

- [ ] Абстрактный интерфейс ITransport (вместо прямого UsbInterface)
- [ ] Linux USB: libusb вместо CH375DLL (для АКИП)
- [ ] Кросс-платформенный CMake (убрать Windows-specific флаги -luser32 и т.д.)
- [ ] AppImage для Linux, .dmg для macOS

---

## Фаза 8 — Расширяемость 🔲
**Статус: Концепт — долгосрочно**

- [ ] Plugin система: добавление нового устройства без перекомпиляции
  ```
  plugins/
    gratten_ga1484.dll  ← отдельный плагин для GA1484
    akip_3409.dll       ← другая модель АКИП
  ```
- [ ] VISA-совместимость (IVI-COM/IVI-C) для интеграции с LabVIEW/MATLAB
- [ ] REST API: HTTP-сервер для управления из браузера/Python

---

## Архитектурные решения (уже принятые)

| Решение | Обоснование |
|---|---|
| `IAkipController` как единый интерфейс | Полиморфизм: MainWindow не знает какое устройство |
| `AbstractScpiController` для SCPI-устройств | Переиспользование кода верификации и словаря команд |
| `SettingsManager` синглтон | Единая точка доступа к настройкам, нет хардкода |
| `gratten_limits.h` словарь команд | Данные отделены от логики, легко расширять |
| Mermaid для диаграмм | Рендерится в GitHub без внешних инструментов |

---

## Метрики "готовности к продукту"

| Критерий | Статус |
|---|---|
| Все баги legacy-кода исправлены | ✅ |
| Нет хардкодов IP/портов | ✅ |
| FM-модуляция работает | ✅ (реализована, требует проверки с железом) |
| Приложение собирается без ошибок | ⏳ (проверить после добавления FM) |
| Все UI-строки в tr() | ❌ |
| Есть unit-тесты | ❌ |
| Есть инсталлятор | ❌ |
| Поддержка RU + EN | ❌ |
