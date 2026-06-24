# Лекция 2. Паттерн «Интерфейс + реализации» в C++17

## Проблема, которую решает паттерн

Представь: приложение должно работать с двумя разными приборами — AKIP-3417 (USB, протокол CH375) и Gratten GA1483 (LAN/TCP). У них похожие команды, но разный транспорт и немного разный диалект SCPI. Если писать отдельный UI для каждого — дублирование кода. Если добавить `if (deviceType == AKIP)` везде — код превращается в клубок.

Решение: **вынести контракт в интерфейс, скрыть реализацию за ним**.

## Чистый абстрактный класс в C++

В C++ интерфейс — это класс, все методы которого чисто виртуальные (`= 0`):

```cpp
class IAkipController : public QObject
{
    Q_OBJECT
public:
    virtual bool openDevice(int index = 0) = 0;
    virtual void closeDevice() = 0;
    virtual bool setFrequency(int channel, double freqHz) = 0;
    virtual QString getIdentity() = 0;
    // ... ещё ~50 методов
    virtual ~IAkipController() {}

signals:
    void opened();
    void availabilityChanged(bool available);
    void errorOccurred(const QString &error);
};
```

Обрати внимание: `IAkipController` наследует `QObject` — это позволяет ему иметь сигналы. В C++ нельзя иметь два `QObject` в цепочке (diamond problem с MOC). Поэтому конкретные реализации наследуют только `IAkipController`, а не `QObject` напрямую.

## Иерархия классов проекта

```
QObject
└── IAkipController           (интерфейс, ~50 чистых виртуальных методов)
    └── AbstractScpiController (частичная реализация: кэши, таймеры, словарь команд)
        ├── AkipFacade         (USB-транспорт через CH375)
        └── GrattenGa1483Controller  (LAN-транспорт через QTcpSocket)
```

`AbstractScpiController` — промежуточный слой. Он предоставляет общую инфраструктуру: кэши значений, словарь команд с проверкой диапазонов, методы замера времени:

```cpp
// В abstractscpicontroller.h:
bool sendCommandTimed(const QString &cmd, qint64 &elapsedMs);
bool queryCommandTimed(const QString &cmd, QString &response, qint64 &elapsedMs);

QMap<int, double> m_freqCache;
QMap<int, bool>   m_outputCache;
```

## Как MainWindow работает с интерфейсом

```cpp
// mainwindow.h
IAkipController *m_controller;  // указатель на интерфейс, не на конкретный класс

// mainwindow.cpp — при подключении Gratten:
auto *ctrl = new GrattenGa1483Controller(this);
ctrl->setConnectionParameters(host, port);
m_controller = ctrl;  // сохраняем как IAkipController*

// Вызов одинаков для любого устройства:
m_controller->setFrequency(1, 1000.0);
m_controller->getIdentity();
```

`MainWindow` не знает, с каким конкретно прибором работает. Виртуальная таблица (vtable) вызовет нужную реализацию в рантайме.

## Сравнение с другими языками

| Язык | Интерфейс |
|------|-----------|
| Java | `interface IAkipController { ... }` — ключевое слово |
| Python | `abc.ABC` + `@abstractmethod` — декоратор |
| C++ | Класс с `= 0` методами и виртуальным деструктором |

## Проверь себя

1. Почему нельзя создать объект типа `IAkipController controller;`? Что скажет компилятор?
2. Зачем `AbstractScpiController` существует между интерфейсом и конкретными реализациями?
3. Что нужно изменить в `MainWindow`, чтобы добавить поддержку третьего прибора — например, Rigol DG1022?
