# Лекция 4. SCPI-протокол и промышленные приборы: USB vs LAN

## Что такое SCPI и IEEE 488

**SCPI** (Standard Commands for Programmable Instruments) — текстовый стандарт команд для измерительного оборудования, основан на **IEEE 488** (GPIB). Команды похожи на путь в дереве настроек:

```
SOURCE1:FREQUENCY 1000000    — установить частоту канала 1 = 1 МГц
SOURCE1:FREQUENCY?           — запросить текущую частоту
*IDN?                        — запросить идентификатор прибора
*RST                         — сброс в заводские настройки
OUTP1 ON                     — включить выход канала 1
```

Знак `?` превращает команду в запрос. Это соглашение неизменно для всех SCPI-приборов.

## Стандартные команды IEEE 488.2

| Команда | Значение |
|---------|----------|
| `*IDN?` | Identification — производитель, модель, серийный номер |
| `*RST`  | Reset — сброс в заводские настройки |
| `*CLS`  | Clear Status — очистить регистры статуса |
| `*OPC?` | Operation Complete — ждать завершения |
| `SYST:ERR?` | Запросить системную ошибку |

В проекте `GrattenGa1483Controller::openDevice()` использует `*IDN?` для верификации подключения:

```cpp
QString idn = queryCommand("*IDN?");
m_available = !idn.isEmpty();
if (!m_available)
    emit errorOccurred("Device opened but does not respond to *IDN?");
```

Если TCP-соединение есть, но `*IDN?` не отвечает — это не «наш» прибор. Защита от случайного подключения к другому устройству на порту 5025.

## Два транспорта: USB (CH375) и LAN (QTcpSocket)

**USB через CH375** (`UsbInterface`, `AkipFacade`):
- CH375 — специализированная микросхема USB-контроллера
- Работает через Windows DLL (`CH375DLL64.dll`) и нативный API
- Команды как бинарные пакеты через WinAPI-вызовы

**LAN через QTcpSocket** (`LanInterface`, `GrattenGa1483Controller`):
```cpp
bool LanInterface::sendScpiCommand(const QString &command)
{
    QByteArray data = command.toLatin1() + "\r\n";  // SCPI требует CRLF
    m_socket->write(data);
    m_socket->flush();
    return true;
}
```

SCPI по TCP — просто текст через обычный сокет. Стандартный порт 5025. `"\r\n"` — обязательный терминатор кадра по SCPI-стандарту (именно строка, не мультисимвольный литерал `'\r\n'`!).

## Диагностика задержек

Реальные приборы могут тормозить: USB-CH375 добавляет до 200 мс, LAN зависит от сети. В проекте есть инструмент измерения:

```cpp
// AbstractScpiController:
bool sendCommandTimed(const QString &cmd, qint64 &elapsedMs);
bool queryCommandTimed(const QString &cmd, QString &response, qint64 &elapsedMs);

// DiagnosticsDialog использует:
qint64 elapsed = 0;
QString response;
m_controller->queryCommandTimed("*IDN?", response, elapsed);
logMessage(QString("*IDN? → %1 мс, ответ: %2").arg(elapsed).arg(response));
```

Если `*IDN?` отвечает за 50 мс — норма. Если за 2000 мс — проблема в транспорте.

## Словарь команд с проверкой диапазонов

`AbstractScpiController` регистрирует команды с метаданными:

```cpp
struct CommandDef {
    QString setPattern;  // "SOURCE%1:FREQ %2"
    QString queryCmd;    // "SOURCE%1:FREQ?"
    double minVal;       // 1.0 Гц
    double maxVal;       // 30e6 Гц
    QString unit;        // "Hz"
    bool isBoolean;      // для ON/OFF команд
};
```

Это позволяет проверить диапазон перед отправкой и не «ломать» прибор некорректными значениями.

## Проверь себя

1. Чем команда `SOURCE1:FREQUENCY 1000000` отличается от `SOURCE1:FREQUENCY?`? Что вернёт каждая?
2. Почему строка дополняется `"\r\n"`, а не просто `"\n"`? Что меняется с точки зрения SCPI-стандарта?
3. Один прибор Gratten можно подключить и по USB, и по LAN. Что нужно изменить в архитектуре, чтобы поддержать оба варианта одновременно?
