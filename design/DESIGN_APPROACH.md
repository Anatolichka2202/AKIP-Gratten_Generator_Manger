# Дизайн-подход

## .ui файлы vs чистый Qt C++

### Текущее состояние
Проект использует 5 `.ui` файлов:
`mainwindow.ui`, `channelwidget.ui`, `grattencontrolwidget.ui`,
`settingsdialog.ui`, `sweepdialog.ui`, `modulationdialog.ui`, `aboutdialog.ui`

---

### Аргументы за .ui

| + | Пояснение |
|---|---|
| Visual editor | Qt Designer — WYSIWYG |
| Separation of concerns | XML-разметка отдельно от логики |
| Уже есть | 5+ файлов написаны, работают |
| AUTOUIC | CMake сам запускает uic |
| Простые диалоги | AboutDialog, SettingsDialog не меняются динамически |

### Аргументы за чистый C++

| + | Пояснение |
|---|---|
| Динамика | show/hide секций, анимации — нативно |
| QSS | тема применяется программно, не зависит от .ui |
| Reuse | компоненты через наследование и шаблоны |
| IDE | полный рефакторинг, нет "мёртвых" имён виджетов |
| Условная вёрстка | AKIP vs Gratten — разный layout без stackedWidget |

---

## Рекомендация: Гибридный подход

```
Оставить .ui                  Переписать на C++
─────────────────────          ───────────────────────────
SettingsDialog.ui             MainWindow (логика + layout)
AboutDialog.ui                GrattenControlWidget
SweepDialog.ui                ChannelWidget (простой, 50 строк)
ModulationDialog.ui
```

**Почему GrattenControlWidget на C++:**
Нужны скрываемые секции FM/PM/Sweep/PULM. В .ui это делается через
`setVisible(false)` на `QGroupBox`, но вся вёрстка фиксирована в XML.
В C++ — добавляем `CollapsibleSection` виджет и управляем им программно.

**Почему MainWindow на C++:**
`stackedWidget` → toolbar → statusbar → тема — это уже программная логика.
.ui остаётся только как декоратор для мёртвых placeholder-ов.

---

## Архитектура UI-слоя (целевая)

```
MainWindow (C++)
├── QToolBar                    — Подключить / IDN / Настройки
├── QStackedWidget
│   ├── AkipPage (C++)          — ChannelWidget × 2
│   └── GrattenPage (C++)       — GrattenControlWidget
├── QStatusBar                  — устройство | IP | последняя команда
└── LogView (QTextEdit)
```

```
GrattenControlWidget (C++)
├── BasicSection (freq, ampl, output, waveform)   — всегда видна
└── CollapsibleSection × 4                        — по кнопке "▼ Расширенные"
    ├── FM (deviation, rate, ON/OFF)
    ├── PM (deviation, rate, ON/OFF)
    ├── Sweep (start, stop, dwell, ON/OFF)
    └── PULM (period, width, ON/OFF)
```

---

## Стиль и тема

### Референсы (измерительное ПО, тёмный стиль)
- **RIGOL UltraScope** — тёмный фон #1A1A2E, акцент #4ECCA3 (зелёный)
- **Keysight BenchVue** — светлый, grid-based, профессиональный
- **LabVIEW** — слишком перегружен, не референс

### Палитра (предложение)
```
Тёмная тема:
  Background:   #1E1E2E   (глубокий тёмно-синий)
  Surface:      #2A2A3E   (карточки, GroupBox)
  Primary:      #89B4FA   (синий, кнопки действия)
  Success:      #A6E3A1   (зелёный, подключено)
  Error:        #F38BA8   (красный, ошибка)
  Text:         #CDD6F4   (основной текст)
  SubText:      #6C7086   (метки, подписи)

Светлая тема:
  Background:   #EFF1F5
  Surface:      #FFFFFF
  Primary:      #1E66F5
  Success:      #40A02B
  Error:        #D20F39
  Text:         #4C4F69
```

### QSS структура
```
design/style/
  dark.qss    — тёмная тема
  light.qss   — светлая тема
  common.qss  — общие правила (шрифт, отступы)
```
