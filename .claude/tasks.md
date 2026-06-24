# Задачи проекта — AKIP/Gratten Generator Manager

> Последнее обновление: 2026-06-24

## Фаза 0 — Workspace [ГОТОВО]
- [x] Анализ legacy-кода
- [x] Создание .claude/ контекста
- [x] README.md, docs/ структура

## Фаза 1 — Исправление ядра [ГОТОВО]
- [x] BUG-001..007: все баги канала B исправлены (рефакторинг в ChannelWidget)
- [x] SettingsManager (QSettings wrapper, без хардкодов)
- [x] SettingsDialog (IP, порт, таймауты Gratten)
- [x] version.h с макросами APP_VERSION_STRING / APP_NAME

## Фаза 2 — Standalone [ГОТОВО]
- [x] Иконка приложения (design/icons/app.ico + app.rc + resources.qrc)
- [x] QSettings: IP, порт, последний тип устройства, язык
- [x] SplashWidget (экран без устройства, quick-reconnect)
- [x] Статическая сборка: build_qt_static.ps1 + CMakePresets.json
- [x] Портабельный деплой: deploy.ps1 (EXE + CH375DLL64.dll)

## Фаза 3 — i18n [ГОТОВО]
- [x] Все UI-строки обёрнуты в tr()
- [x] translations/ru_RU.ts + en_US.ts (полные)
- [x] LanguageSwitcher + меню "Настройки → Язык"
- [x] CMake: qt6_add_translations, fallback для LinguistTools
- [x] Смена языка в runtime (changeEvent/retranslateUi во всех виджетах)

## Фаза 4 — UI/UX [ЧАСТИЧНО]
- [x] StatusBar с текущими параметрами подключения
- [x] SplashWidget интегрирован (показывается при старте/дисконнекте)
- [x] ModulationDialog подключён к GrattenControlWidget
- [x] DiagnosticsDialog (Инструменты → Диагностика)
- [x] Catppuccin dark/light темы (ThemeManager)
- [x] Меню: Настройки / Инструменты / Справка
- [ ] Toolbar с быстрыми действиями (backlog)

## Фаза 5 — Тесты [ЧАСТИЧНО]
- [x] MockTransport для тестирования без железа
- [x] test_gratten_controller: IDN, setFreq, queryFreq, setAmplitude
- [x] test_settings_manager: defaults, roundtrips, language, timeouts
- [x] test_mock_transport: базовый тест
- [ ] test_akip_facade: unit-тесты для USB-контроллера
- [ ] CI (GitHub Actions): автозапуск тестов на push

## Фаза 6 — Деплой [В РАБОТЕ]
- [x] qDebug заглушён в Release (QT_NO_DEBUG)
- [x] Статическая Qt 6.8 собрана из Src/ (C:\Qt\6.8.0-static\mingw_64)
- [x] Portable EXE: 43 МБ, только Windows API + CH375DLL64.dll
- [ ] Подписать EXE (опционально, нужен сертификат)
- [ ] Installer: NSIS / Qt IFW (опционально)

## Backlog
- Поддержка нескольких одновременных устройств
- Запись/воспроизведение последовательностей команд
- Cross-platform USB (libusb вместо CH375DLL)
- FM/sweep/burst для АКИП-3417 (нужна документация по командам)
- CI/CD (GitHub Actions)
- API-документация (Doxygen)
