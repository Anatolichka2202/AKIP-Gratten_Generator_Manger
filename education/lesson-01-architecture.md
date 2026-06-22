# Урок 1: Как устроен твой код — Паттерны архитектуры

## Что ты сделал (молодец!)

Твой код полугодичной давности уже использует правильные архитектурные паттерны — ты сам до них дошёл. Давай разберём что это такое и зачем.

---

## 1. Интерфейс (Interface / Pure Abstract Class)

**Файл:** `iakipcontroller.h`

```cpp
class IAkipController : public QObject {
public:
    virtual bool setFrequency(int channel, double freqHz) = 0;  // чисто виртуальный
    virtual double queryFrequency(int channel) = 0;
    // ...
};
```

`= 0` — это признак **чисто виртуального метода**. Класс с такими методами нельзя создать напрямую (`new IAkipController()` — ошибка компиляции).

**Зачем это нужно:**
Остальной код (MainWindow) работает только с `IAkipController*` и не знает, это АКИП или Граттен. Это называется **полиморфизм** — одинаковый код работает с разными объектами.

```cpp
// MainWindow не знает, что именно подключено:
IAkipController *m_controller;
m_controller->setFrequency(1, 1000.0); // работает для обоих устройств
```

---

## 2. Facade паттерн

**Файл:** `akipfacade.h/cpp`

`AkipFacade` скрывает сложность `UsbInterface` за простым интерфейсом.

```
Снаружи видно:        setFrequency(1, 1000.0)
Внутри происходит:    m_usb.sendScpiCommand("FREQ:CHA 1000")
```

Представь фасад здания — снаружи красиво, что за ним — не твоя забота.

---

## 3. Template Method паттерн

**Файл:** `abstractscpicontroller.h/cpp`

`AbstractScpiController` определяет **общий алгоритм** с шагами, которые реализуют наследники:

```cpp
// Базовый класс определяет КАК верифицировать команду:
bool verifySetCommand(cmd, queryFunc, expectedValue) {
    sendCommand(cmd);           // ← реализует наследник
    double got = queryFunc();   // ← тоже наследник
    return qAbs(got - expectedValue) < tolerance;
}
```

Граттен реализует `sendCommand` через TCP, будущий прибор через RS-232 — алгоритм верификации не меняется.

---

## 4. Command Dictionary

**Файл:** `abstractscpicontroller.h`, структура `CommandDef`

```cpp
struct CommandDef {
    QString setPattern;   // ":FREQuency:CW %1Hz"
    QString queryCmd;     // ":FREQuency:CW?"
    double minVal;        // 250000.0
    double maxVal;        // 4000000000.0
    // ...
};
```

Данные (что именно посылать устройству) отделены от логики (как посылать). Менять параметры команд можно без правки алгоритмов.

---

## Вывод

Ты интуитивно применил 4 паттерна. Теперь ты знаешь их названия:
1. **Interface** — `IAkipController` → полиморфизм
2. **Facade** — `AkipFacade` → скрыть сложность
3. **Template Method** — `AbstractScpiController` → общий алгоритм, переменные шаги
4. **Command Dictionary** — `CommandDef` → данные отдельно от логики

---

## Что почитать дальше
- "Паттерны проектирования" — Банда четырёх (GoF) — классика
- Qt documentation: "Signals & Slots" — уникальная система Qt для событий
