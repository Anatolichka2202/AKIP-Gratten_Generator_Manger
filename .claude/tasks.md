# Задачи проекта

## Фаза 0 — Подготовка workspace [ТЕКУЩАЯ]
- [x] Анализ legacy-кода
- [x] Создание .claude/ контекста
- [x] README.md
- [x] docs/ структура
- [x] design/ папка
- [x] education/ папка
- [ ] Найти документацию АКИП-3417
- [ ] Найти документацию Gratten GA1483

## Фаза 1 — Исправление ядра [В РАБОТЕ]
- [x] BUG-001: on_btnSetOutputB reads chkOutputA → chkOutputB
- [x] BUG-002: on_btnQueryFreqB writes to editFreqA → editFreqB
- [x] BUG-003: on_btnSetAmplB reads editAmplA/cmbAmplUnitA → B-поля
- [x] BUG-004: on_btnSetWaveB reads cmbWaveformA → cmbWaveformB
- [x] BUG-005: IP Gratten захардкожен → SettingsManager
- [x] BUG-006: setAMState — улучшена заглушка (команда AM:STAT, TODO для проверки с железом)
- [x] BUG-007: on_btnQueryOutputB writes to chkOutputA → chkOutputB
- [x] Создан SettingsManager (QSettings wrapper)
- [x] Создан SettingsDialog (IP, порт, таймауты Gratten)
- [x] Добавлено меню "Настройки → Подключение..."
- [x] Переименован проект в CMakeLists.txt
- [ ] version.h с макросами версии
- [ ] Дождаться результатов ресерча по командам (agent: ace6d7e32b7597fe5)

## Фаза 2 — Standalone
- [ ] Переименовать project в CMakeLists.txt
- [ ] Добавить `app_info.h` (название, версия, организация)
- [ ] Добавить иконку приложения
- [ ] QSettings: persist IP, порт, последний тип устройства

## Фаза 3 — i18n
- [ ] Обернуть все строки в tr()
- [ ] Создать .ts файлы (ru, en)
- [ ] Добавить LanguageSwitcher в меню
- [ ] Настроить CMake для lupdate/lrelease

## Фаза 4 — UI/UX
- [ ] Дизайн-макет (см. design/)
- [ ] Рефакторинг MainWindow (слишком большой, 717 строк)
- [x] Добавить StatusBar с текущими параметрами
- [x] SplashWidget интегрирован в MainWindow (показывается при старте/дисконнекте)
- [x] ModulationDialog подключён к GrattenControlWidget (кнопка Модуляция...)
- [ ] Добавить toolbar

## Фаза 5 — Тесты и документация
- [ ] Unit-тесты (Qt Test) для AkipFacade, GrattenController
- [ ] Mock-транспорт для тестирования без железа
- [ ] API-документация (Doxygen)
- [ ] Обновить README.md

## Backlog
- Поддержка нескольких одновременных устройств
- Запись/воспроизведение последовательностей команд
- Экспорт конфигурации в JSON
- Cross-platform USB (libusb вместо CH375DLL)
