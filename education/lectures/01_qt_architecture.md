# Лекция 1. Архитектура Qt-приложения: QObject, сигналы/слоты, иерархия виджетов

## Что такое QObject и зачем он нужен

Почти всё в Qt наследуется от `QObject`. Это не просто базовый класс — он несёт три фундаментальных механизма: систему мета-объектов (MOC), сигналы и слоты, управление временем жизни через дерево родитель/потомок.

Чтобы класс стал «настоящим» Qt-объектом, достаточно двух вещей:
```cpp
class MainWindow : public QMainWindow  // QMainWindow -> QWidget -> QObject
{
    Q_OBJECT   // <-- команда MOC-генератору
    ...
};
```

Макрос `Q_OBJECT` запускает Meta-Object Compiler. MOC читает заголовочный файл и генерирует дополнительный `.cpp` с метаданными: именами сигналов, слотов, типов. Без `Q_OBJECT` сигналы и слоты работать не будут — и это выразится в загадочных ошибках линковщика, а не компилятора.

## Сигналы и слоты: связь без жёсткой зависимости

В нашем проекте это видно в `MainWindow::MainWindow()`:
```cpp
// SplashWidget не знает ничего о MainWindow — просто испускает сигнал
connect(m_splashPage, &SplashWidget::connectRequested,
        this, &MainWindow::on_btnConnect_clicked);

// Lambda как слот — удобно для маленьких действий
connect(m_splashPage, &SplashWidget::settingsRequested, this, [this]() {
    SettingsDialog dlg(this);
    dlg.exec();
});
```

Синтаксис с `&ClassName::method` (указатель на член) появился в Qt 5 и проверяется **при компиляции**. Старый `SIGNAL()/SLOT()` работал через строки и падал только в рантайме. Всегда используйте новый синтаксис.

## Иерархия виджетов и RAII через parent

Qt реализует автоматическое управление памятью через дерево объектов: родитель уничтожает потомков рекурсивно.

```cpp
// Оба ChannelWidget будут удалены вместе с MainWindow
m_channelA = new ChannelWidget(1, nullptr, this);  // this = parent
m_channelB = new ChannelWidget(2, nullptr, this);
```

Но не всегда есть родитель. В деструкторе `MainWindow`:
```cpp
MainWindow::~MainWindow()
{
    if (m_controller) {
        m_controller->closeDevice();  // сначала корректно закрываем прибор
        delete m_controller;          // потом освобождаем память
    }
    delete ui;  // ui создаётся без parent — удаляем вручную
}
```

`m_controller` создаётся динамически при подключении и может меняться (AKIP → Gratten), поэтому его время жизни контролируется вручную.

## Архитектура виджетов проекта

```
MainWindow (QMainWindow)
├── QStackedWidget
│   ├── AKIP page (из .ui-файла)
│   │   ├── ChannelWidget [канал A]
│   │   └── ChannelWidget [канал B]
│   ├── GrattenControlWidget
│   └── SplashWidget
└── QStatusBar
    ├── m_sbDevice  (QLabel)
    ├── m_sbConn    (QLabel)
    └── m_sbLastOp  (QLabel)
```

`QStackedWidget` показывает один дочерний виджет за раз — стандартный способ переключаться между «экранами» без отдельных окон.

## Проверь себя

1. Что произойдёт, если убрать макрос `Q_OBJECT` из класса с сигналами? Будет ошибка компиляции или линковки?
2. В чём разница между `new ChannelWidget(this)` и `new ChannelWidget(nullptr)`? Когда каждый вариант уместен?
3. Почему `m_controller` объявлен как `IAkipController*`, а не `AkipFacade*`?
