# Education — Режим ментора

Здесь собираются учебные материалы, объяснения и концепции, которые разбираются по ходу разработки проекта.

## Темы для изучения

### Qt Framework
- [ ] Система сигналов и слотов (Qt Signals & Slots)
- [ ] QObject: ownership, parent-child, memory management
- [ ] QWidget, QMainWindow, QDialog
- [ ] Виджеты: QStackedWidget, QGroupBox, QComboBox
- [ ] QElapsedTimer, QTimer, QThread
- [ ] QSettings — персистентное хранение настроек
- [ ] Qt Internationalization (QTranslator, tr(), lupdate, lrelease)
- [ ] Qt Designer (.ui файлы) и AUTOUIC

### C++17
- [ ] Умные указатели (unique_ptr, shared_ptr)
- [ ] std::function, лямбды
- [ ] if constexpr, structured bindings
- [ ] RAII паттерн

### Архитектурные паттерны
- [ ] Интерфейс (pure virtual) vs. абстрактный класс
- [ ] Facade паттерн (AkipFacade)
- [ ] Template Method (AbstractScpiController)
- [ ] Command Dictionary паттерн (CommandDef)

### Протокол SCPI
- [ ] Что такое SCPI
- [ ] Структура команд (IEEE 488.2)
- [ ] *IDN?, *RST, *CLS
- [ ] Иерархия команд (FREQuency:CW)
- [ ] Query vs Set команды

### Сетевое программирование
- [ ] TCP сокеты в Qt (QTcpSocket)
- [ ] SCPI-over-LAN (порт 5025, VXI-11 vs raw socket)
- [ ] Обработка таймаутов

### USB программирование (Windows)
- [ ] HID vs Bulk transfer
- [ ] CH375 chip: что это и зачем
- [ ] Windows.h и HANDLE

## Уроки (добавляются по ходу)

Каждый урок — отдельный файл в этой папке.
