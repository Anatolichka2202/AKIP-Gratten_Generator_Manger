# Техническая архитектура

## Диаграмма классов

```
QObject
└── IAkipController (abstract)
    ├── signals: opened, closed, availabilityChanged, errorOccurred
    │            frequencyChanged, outputChanged, amplitudeChanged,
    │            waveformChanged, dutyCycleChanged, amFrequencyChanged,
    │            amDepthChanged, amStateChanged
    │
    ├── AkipFacade
    │   └── has-a: UsbInterface (QObject → CH375DLL)
    │
    └── AbstractScpiController (abstract)
        ├── has: QMap caches (freq, output, ampl, wave, AM…)
        ├── has: QMap<QString, CommandDef> m_commandMap
        └── GrattenGa1483Controller
            └── has-a: LanInterface (QObject → QTcpSocket)

QMainWindow
└── MainWindow
    ├── has-a: IAkipController* m_controller (owned, polymorphic)
    ├── has-a: QStackedWidget (page 0 = AKIP UI, page 1 = Gratten UI)
    └── has-a: GrattenControlWidget* m_grattenPage (optional)
```

## Транспортный слой

### USB (АКИП-3417)
- Драйвер: CH375 USB bulk transfer chip
- DLL: `CH375DLL64.dll` (Windows 64-bit only)
- Протокол: SCPI-like, нестандартный синтаксис команд (FREQ:CHA, OUTP:CHB и т.д.)
- Таймаут записи/чтения: настраивается в UsbInterface

### LAN (Gratten GA1483)
- Порт: TCP 5025 (стандарт SCPI-over-LAN / VXICPL-11)
- Транспорт: QTcpSocket
- Таймаут подключения: 5000 мс (по умолчанию)
- Таймаут ответа: 3000 мс (по умолчанию)

## Жизненный цикл контроллера

```
MainWindow::checkAvailableDevices()
  → Probe AKIP (USB open + *IDN?)
  → Probe Gratten (TCP connect + *IDN?)
  → Decision: один/оба/никого
  → MainWindow::switchToDevice(type)
      → delete old controller
      → new AkipFacade / GrattenGa1483Controller
      → connect signals
      → controller->openDevice()
      → setupForDeviceType(type)
```

## Кеширование

`AbstractScpiController` держит QMap-кеши для всех параметров каналов.
Кеш обновляется при каждом успешном `set*()` или `query*()`.
Цель: минимизировать лишние запросы к устройству.

## Верификация команд

`AbstractScpiController::verifySetCommand()` — после отправки команды выполняет обратный запрос
и сравнивает значение. Повторяет до `maxRetries` раз с задержкой `delayMs`.

## Словарь команд Gratten

`gratten_limits.h` — inline `QVector<GrattenCommandInfo>` с паттернами команд,
диапазонами значений и флагами булевости.
`AbstractScpiController::registerCommand()` / `buildSetCommand()` строят команды
из этого словаря.

## Известные ограничения

1. **Только Windows**: CH375 SDK — Windows-only. Для Linux/macOS нужен libusb.
2. **IP Gratten захардкожен**: `"192.168.1.66"` в `mainwindow.cpp:9`. Нужны QSettings.
3. **АКИП SCPI нестандартный**: `FREQ:CHA 1000` вместо `[:SOURce]:FREQuency 1000 HZ`.
4. **Один контроллер**: архитектура позволяет только одно устройство одновременно.
5. **Нет i18n**: все строки жёстко в коде на русском.
