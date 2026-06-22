# Найденные баги в legacy-коде

## BUG-001 — Канал B: SetOutput читает чекбокс канала A
**Файл:** `mainwindow.cpp:299`
**Серьёзность:** High
```cpp
// БЫЛО (баг):
bool enable = ui->chkOutputA->isChecked(); // ← должно быть chkOutputB
// ДОЛЖНО БЫТЬ:
bool enable = ui->chkOutputB->isChecked();
```

## BUG-002 — Канал B: QueryFreq пишет в поле канала A
**Файл:** `mainwindow.cpp:284`
**Серьёзность:** High
```cpp
// БЫЛО (баг):
ui->editFreqA->setText(QString::number(freq)); // ← должно быть editFreqB
// ДОЛЖНО БЫТЬ:
ui->editFreqB->setText(QString::number(freq));
```

## BUG-003 — Канал B: SetAmpl читает поля канала A (2 места)
**Файл:** `mainwindow.cpp:334,339`
**Серьёзность:** High
```cpp
// БЫЛО (баг):
double ampl = ui->editAmplA->text().toDouble();   // ← editAmplB
QString unit = ui->cmbAmplUnitA->currentText();    // ← cmbAmplUnitB
```

## BUG-004 — Канал B: SetWaveform читает комбобокс канала A
**Файл:** `mainwindow.cpp:377`
**Серьёзность:** Medium
```cpp
// БЫЛО (баг):
QString wave = ui->cmbWaveformA->currentText(); // ← cmbWaveformB
```

## BUG-005 — IP Gratten захардкожен
**Файл:** `mainwindow.cpp:9`
**Серьёзность:** High (блокирует использование на другом стенде)
```cpp
const QString ipgratten = "192.168.1.66"; // ← должно браться из QSettings
```

## BUG-006 — setAMState — заглушка, не отправляет команду
**Файл:** `akipfacade.cpp:353-357`
**Серьёзность:** Medium
```cpp
// Сейчас: просто emit amStateChanged, команда не отправляется
// Нужно: отправить реальную SCPI-команду (уточнить по документации АКИП-3417)
```

## BUG-007 — QueryOutputB читает chkOutputA для установки состояния
**Файл:** `mainwindow.cpp:322`
**Серьёзность:** Medium
```cpp
ui->chkOutputA->setChecked(on); // ← chkOutputB
```
