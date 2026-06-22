# Архитектурные диаграммы

Все диаграммы в формате [Mermaid](https://mermaid.js.org/) — рендерятся в GitHub, Obsidian, VS Code.

---

## 1. Иерархия классов

```mermaid
classDiagram
    class IAkipController {
        <<interface>>
        +openDevice(index) bool
        +closeDevice()
        +isOpen() bool
        +isAvailable() bool
        +setFrequency(ch, hz) bool
        +setOutput(ch, enable) bool
        +setAmplitude(ch, amp, unit) bool
        +setWaveform(ch, wave) bool
        +setDutyCycle(ch, pct) bool
        +setAMFrequency(ch, hz) bool
        +setAMDepth(ch, pct) bool
        +setAMState(ch, en) bool
        +setFMFrequency(ch, hz) bool
        +setFMDeviation(ch, hz) bool
        +setFMState(ch, en) bool
        +getIdentity() QString
        +reset() bool
        +sendCommand(cmd) bool
        +queryCommand(cmd) QString
    }

    class AbstractScpiController {
        <<abstract>>
        #m_freqCache QMap
        #m_amplCache QMap
        #m_fmFreqCache QMap
        #m_fmDevCache QMap
        #m_commandMap QMap~CommandDef~
        +registerCommand(key, def)
        +buildSetCommand(key, val) QString
        +buildQueryCommand(key) QString
        +checkRange(key, val) bool
        +verifySetCommand(cmd, queryFn, expected) bool
    }

    class AkipFacade {
        -m_usb UsbInterface
        -m_available bool
        +openDevice() bool
        +setFrequency() bool
        +setFMFrequency() bool
    }

    class GrattenGa1483Controller {
        -m_lan LanInterface
        -m_host QString
        -m_port quint16
        +setConnectionParameters(host, port)
        +setFrequency() bool
        +setFMFrequency() bool
        +setFMDeviation() bool
        +setFMState() bool
    }

    class UsbInterface {
        -m_deviceHandle HANDLE
        +open(index) bool
        +sendScpiCommand(cmd) bool
        +queryScpiCommand(cmd) QString
    }

    class LanInterface {
        -m_socket QTcpSocket
        +connectToHost(host, port) bool
        +sendScpiCommand(cmd) bool
        +queryScpiCommand(cmd) QString
    }

    IAkipController <|-- AbstractScpiController
    IAkipController <|-- AkipFacade
    AbstractScpiController <|-- GrattenGa1483Controller
    AkipFacade *-- UsbInterface
    GrattenGa1483Controller *-- LanInterface
```

---

## 2. Диаграмма компонентов

```mermaid
graph TB
    subgraph UI ["UI Layer (Qt Widgets)"]
        MW[MainWindow\nmainwindow.h/cpp/.ui]
        GW[GrattenControlWidget\ngrattencontrolwidget.h/cpp/.ui]
        SD[SettingsDialog\nsettingsdialog.h/cpp/.ui]
    end

    subgraph Core ["Business Logic"]
        IF[IAkipController\niakipcontroller.h]
        AF[AkipFacade\nakipfacade.h/cpp]
        ASC[AbstractScpiController\nabstractscpicontroller.h/cpp]
        GC[GrattenGa1483Controller\ngrattenga1483controller.h/cpp]
        SM[SettingsManager\nsettingsmanager.h/cpp]
        GL[gratten_limits.h\ncommand dictionary]
    end

    subgraph Transport ["Transport Layer"]
        USB[UsbInterface\nusbinterface.h/cpp]
        LAN[LanInterface\nlaninterface.h/cpp]
    end

    subgraph HW ["Hardware"]
        AKIP[АКИП-3417\nUSB / CH375]
        GRTN[Gratten GA1483\nLAN TCP:5025]
    end

    subgraph SDK ["External SDK"]
        DLL[CH375DLL64.dll\nch375_sdk/]
    end

    MW --> IF
    MW --> SD
    MW --> GW
    GW --> IF
    SD --> SM
    MW --> SM

    IF <|-- AF
    IF <|-- ASC
    ASC <|-- GC
    GC --> GL

    AF --> USB
    GC --> LAN

    USB --> DLL
    USB --> AKIP
    LAN --> GRTN
    DLL --> AKIP
```

---

## 3. Последовательность: подключение к Gratten

```mermaid
sequenceDiagram
    actor User
    participant MW as MainWindow
    participant SM as SettingsManager
    participant GC as GrattenGa1483Controller
    participant LAN as LanInterface
    participant DEV as Gratten GA1483

    User->>MW: Запуск приложения
    MW->>MW: QTimer::singleShot(100ms) → checkAvailableDevices()
    MW->>SM: grattenHost(), grattenPort()
    SM-->>MW: "192.168.1.66", 5025

    MW->>GC: new GrattenGa1483Controller()
    MW->>GC: setConnectionParameters(host, port)
    MW->>GC: openDevice()
    GC->>LAN: connectToHost(host, port, 5000ms)
    LAN->>DEV: TCP SYN → SYN-ACK → ACK
    LAN-->>GC: connected()

    GC->>LAN: queryScpiCommand("*IDN?")
    LAN->>DEV: "*IDN?\n"
    DEV-->>LAN: "Atten,GA1483,SN...\n"
    LAN-->>GC: "Atten,GA1483,..."
    GC->>GC: m_available = true
    GC-->>MW: emit opened() + availabilityChanged(true)
    MW->>MW: setupForDeviceType(GRATTEN)
    MW->>MW: lblIdn.setText(idn)
```

---

## 4. Машина состояний: контроллер устройства

```mermaid
stateDiagram-v2
    [*] --> Disconnected : создан объект

    Disconnected --> Connecting : openDevice()
    Connecting --> Connected : TCP OK + IDN? ответил
    Connecting --> Disconnected : таймаут / ошибка сети

    Connected --> Available : m_available = true\navailabilityChanged(true)
    Available --> Busy : отправка команды
    Busy --> Available : команда выполнена
    Busy --> Error : таймаут / верификация провалена

    Error --> Available : следующая команда (автовосстановление)
    Error --> Disconnected : потеря соединения

    Available --> Disconnected : closeDevice()
    Connected --> Disconnected : closeDevice()

    note right of Busy
        verifySetCommand():
        1. send cmd
        2. sleep 200ms
        3. query back
        4. compare
    end note
```

---

## 5. Поток данных: установка FM-модуляции

```mermaid
flowchart LR
    UI[GrattenControlWidget\nsetFMState ON\ndev=75kHz\nfreq=1kHz]
    -->|setFMState| GC[GrattenGa1483Controller]
    GC -->|buildSetCommand\nFMSTATE| DICT[gratten_limits.h\nCommandDef]
    DICT -->|":FM:STATe ON"| GC
    GC -->|verifySetCommand| LAN[LanInterface]
    LAN -->|TCP send| DEV[GA1483]
    DEV -->|sleep 200ms| DEV
    LAN -->|queryFMState?| DEV
    DEV -->|":FM:STATe?"| DEV
    DEV -->|"ON"| LAN
    LAN -->|bool true| GC
    GC -->|emit fmStateChanged| MW[MainWindow\nUI update]
```

---

## 6. Дерево модулей и зависимостей

```mermaid
graph TD
    main.cpp --> MainWindow
    MainWindow --> IAkipController
    MainWindow --> SettingsManager
    MainWindow --> SettingsDialog
    MainWindow --> GrattenControlWidget

    IAkipController --> AkipFacade
    IAkipController --> AbstractScpiController
    AbstractScpiController --> GrattenGa1483Controller

    AkipFacade --> UsbInterface
    GrattenGa1483Controller --> LanInterface
    GrattenGa1483Controller --> gratten_limits.h

    UsbInterface --> CH375DLL64.dll
    LanInterface --> QTcpSocket["Qt::Network\nQTcpSocket"]
    SettingsManager --> QSettings["Qt::Core\nQSettings"]

    style CH375DLL64.dll fill:#f96,stroke:#333
    style QTcpSocket fill:#9cf,stroke:#333
    style QSettings fill:#9cf,stroke:#333
```
