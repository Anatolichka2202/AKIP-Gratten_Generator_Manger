# Лекция 3. Интернационализация Qt: tr(), QTranslator, LanguageSwitcher

## Зачем i18n и как это работает в Qt

Интернационализация (i18n) — подготовка кода к переводу. В Qt цепочка из трёх звеньев:

1. **`tr()`** — помечает строку как переводимую прямо в коде.
2. **`lupdate`** — сканирует исходники, собирает все `tr()` в `.ts` файл (XML).
3. **`lrelease`** — компилирует `.ts` в бинарный `.qm` файл для рантайма.

```cpp
// Правильно — строка попадёт в словарь переводов:
m_sbDevice = new QLabel(tr("Не подключено"), this);

// Неправильно — hardcoded, перевод невозможен:
m_sbDevice = new QLabel("Не подключено", this);
```

## Класс LanguageSwitcher

Отдельный класс инкапсулирует всю логику смены языка:

```cpp
LanguageSwitcher::LanguageSwitcher(QApplication *app, QObject *parent)
    : QObject(parent), m_app(app)
{
    m_currentLocale = SettingsManager::instance().language();
    if (m_currentLocale.isEmpty())
        m_currentLocale = "ru_RU";
    loadLanguage(m_currentLocale);  // загружаем сохранённый язык при старте
}
```

## Загрузка .qm файла

```cpp
void LanguageSwitcher::loadLanguage(const QString &locale)
{
    m_app->removeTranslator(&m_translator);  // снимаем старый переводчик

    QStringList searchPaths = {
        QCoreApplication::applicationDirPath() + "/translations",
        QCoreApplication::applicationDirPath(),
        ":/i18n",         // встроено в ресурсы (qt6_add_translations)
        ":/translations"  // резервный путь
    };

    for (const QString &path : searchPaths) {
        if (m_translator.load(locale, path)) {
            m_app->installTranslator(&m_translator);
            break;
        }
    }
    SettingsManager::instance().setLanguage(locale);
}
```

После `installTranslator` Qt автоматически рассылает `QEvent::LanguageChange` всем виджетам.

## Почему динамические меню не переводятся автоматически

Виджеты из `.ui` файла обновляются через `ui->retranslateUi(this)`. Но меню, созданные вручную в коде, Qt не трогает — он не знает, какие строки там.

Решение — `retranslateMenus()`, вызываемый из `changeEvent`:

```cpp
void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);   // автоматика для .ui виджетов
        retranslateMenus();         // ручной перевод для кодовых меню
        updateConnectionStatus();
        updateStatusBar();
    }
    QMainWindow::changeEvent(event);  // обязательно вызываем базовый класс!
}

void MainWindow::retranslateMenus()
{
    if (m_menuSettings)  m_menuSettings->setTitle(tr("Настройки"));
    if (m_actConnection) m_actConnection->setText(tr("Подключение..."));
    if (m_menuTools)     m_menuTools->setTitle(tr("Инструменты"));
    if (m_actDiag)       m_actDiag->setText(tr("Диагностика..."));
    if (m_menuHelp)      m_menuHelp->setTitle(tr("Справка"));
    if (m_actAbout)      m_actAbout->setText(tr("О программе..."));
}
```

## QActionGroup для взаимного исключения

В меню языков `QActionGroup` гарантирует, что только один язык отмечен галочкой — работает как radio-button группа:

```cpp
QActionGroup *group = new QActionGroup(menu);
group->setExclusive(true);

QAction *ruAction = group->addAction(tr("Русский"));
ruAction->setCheckable(true);
ruAction->setChecked(m_currentLocale == "ru_RU");
```

## Проверь себя

1. Что нужно сделать в цепочке `tr() → lupdate → lrelease`, чтобы добавить немецкий язык?
2. Почему вызов `QMainWindow::changeEvent(event)` в конце обязателен? Что случится без него?
3. Если создать новую `QLabel` с текстом в `retranslateMenus()`, переведётся ли она при следующей смене языка?
