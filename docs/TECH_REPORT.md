# Отчёт техника — состояние кода

## Архитектура (текущая, master)

```
IAkipController          ← единый интерфейс (неудачное имя — не только АКИП)
├── AkipFacade           → USB CH375 (АКИП-3417, 2 канала)
└── AbstractScpiController
    └── GrattenGa1483Controller  → LAN TCP:5025 (Gratten GA1483, 1 канал)

ITransport               ← абстрактный транспорт (добавлен в PR #11)
└── MockTransport        ← для unit-тестов без железа

MainWindow
├── stackedWidget
│   ├── akipPage  →  ChannelWidget(1) + ChannelWidget(2)   [АКИП режим]
│   └── grattenPage  →  GrattenControlWidget               [Gratten режим]
├── SettingsDialog   (IP, порт, таймауты)
├── ModulationDialog (AM/FM управление)
├── SweepDialog      (свип)
├── AboutDialog
└── LanguageSwitcher (RU/EN)

SettingsManager   ← QSettings wrapper (без хардкода)
```

---

## Подстановка интерфейсов по типу устройства

Реализована через `stackedWidget` в `mainwindow.cpp`:

```
При старте: checkAvailableDevices()
  ├── Пробует открыть АКИП (USB index 0)
  ├── Пробует открыть Gratten (LAN, IP из QSettings)
  └── Результат:
       оба     → диалог выбора
       только АКИП   → auto switchToDevice(AKIP)
       только Gratten → auto switchToDevice(GRATTEN)
       ни одно → сообщение, ручной выбор

switchToDevice(type):
  if AKIP:
    stackedWidget → akipPage
    ChannelWidget A + B ← m_controller
  if GRATTEN:
    создаёт GrattenControlWidget(m_controller)
    stackedWidget → grattenPage
```

**Вывод**: подстановка работает. Граттен видит только `GrattenControlWidget` (1 канал). АКИП видит `ChannelWidget A` + `ChannelWidget B`.

---

## Найденные архитектурные проблемы

### P1 — setChannelControlsEnabled всегда трогает m_channelA/B
```cpp
void MainWindow::setChannelControlsEnabled(bool enabled)
{
    m_channelA->setEnabled(enabled);   // ← в Gratten режиме эти виджеты скрыты,
    m_channelB->setEnabled(enabled);   //   но всё равно получают setEnabled
    ui->grpDelays->setEnabled(enabled);
}
```
Не крашится, но логически неверно. В Gratten-режиме нужно управлять `m_grattenPage`.

### P2 — Имя интерфейса IAkipController
`IAkipController` — имя обманывает. Это общий интерфейс для любого SCPI-генератора.  
Правильное имя: `ISignalGenerator`. Задача: переименовать (чисто рефакторинг).

### P3 — GrattenControlWidget не показывает FM/PM/Sweep/PULM
API есть (все методы в `GrattenGa1483Controller`), UI нет.  
Нужно добавить секции (QGroupBox, скрываемые) в `grattencontrolwidget.ui`.

### P4 — tr() не везде
`mainwindow.cpp`: строки частично в `tr()`, частично нет.  
`LanguageSwitcher` создан, меню есть, но переключение языка не повлияет на ~40% строк.

### P5 — Нет StatusBar
Текущий статус (тип устройства, IP, время последней команды) отображается только в `txtLog`.  
Нужен `QStatusBar`.

### P6 — AkipFacade не реализует ITransport
`MockTransport` реализует `ITransport`, но `AkipFacade` напрямую использует `UsbInterface`, минуя абстракцию.  
Unit-тест `AkipFacade` с `MockTransport` невозможен без рефакторинга.

---

## Метрики кода (master)

| Файл | Строк | Комментарий |
|---|---|---|
| mainwindow.cpp | 430 | было 717 в legacy — рефакторинг помог |
| akipfacade.cpp | 434 | большой, но логичный |
| grattenga1483controller.cpp | 428 | полный API, Q_UNUSED(channel) везде |
| abstractscpicontroller.cpp | 174 | верификация команд, sendCommandTimed |
| channelwidget.cpp | 279 | чистый, после фикса QGroupBox |
| **Итого** | ~3300 | 20+ файлов |

---

## CI статус

GitHub Actions (Ubuntu, Qt 6.6): **GREEN** после PR #13.  
Тест: `test_mock_transport` — 1 unit-тест, проходит.  
Windows CI: **не настроен** (CH375 SDK нет на runner'е).

---

## Что работает с железом (ожидаемо)

| Функция | Gratten | АКИП |
|---|---|---|
| Подключение | ✅ LAN | ✅ USB (Windows) |
| Частота | ✅ | ✅ |
| Амплитуда | ✅ (dBm) | ✅ (Vpp) |
| Форма сигнала | ✅ | ✅ |
| Выход ON/OFF | ✅ | ✅ |
| AM модуляция | ✅ | ⚠️ AM:STAT не верифицирован |
| FM модуляция | ✅ | ⚠️ FM:STAT не верифицирован |
| PM / Sweep / PULM | ✅ API | ⚠️ Стаб |
| UI для PM/Sweep/PULM | ❌ нет UI | ❌ нет UI |
