#include "languageswitcher.h"
#include "settingsmanager.h"
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

LanguageSwitcher::LanguageSwitcher(QApplication *app, QObject *parent)
    : QObject(parent)
    , m_app(app)
{
    // Load saved language from settings
    m_currentLocale = SettingsManager::instance().language();
    if (m_currentLocale.isEmpty())
        m_currentLocale = "ru_RU";
    loadLanguage(m_currentLocale);
}

QMenu* LanguageSwitcher::createLanguageMenu(QWidget *menuParent)
{
    QMenu *menu = new QMenu(tr("Язык / Language"), menuParent);

    QActionGroup *group = new QActionGroup(menu);
    group->setExclusive(true);

    QAction *ruAction = group->addAction(tr("Русский"));
    ruAction->setData("ru_RU");
    ruAction->setCheckable(true);
    ruAction->setChecked(m_currentLocale == "ru_RU");
    menu->addAction(ruAction);
    connect(ruAction, &QAction::triggered, this, &LanguageSwitcher::onLanguageActionTriggered);

    QAction *enAction = group->addAction(tr("English"));
    enAction->setData("en_US");
    enAction->setCheckable(true);
    enAction->setChecked(m_currentLocale == "en_US");
    menu->addAction(enAction);
    connect(enAction, &QAction::triggered, this, &LanguageSwitcher::onLanguageActionTriggered);

    return menu;
}

void LanguageSwitcher::loadLanguage(const QString &locale)
{
    m_app->removeTranslator(&m_translator);

    // Look for .qm file in app directory and translations/ subdirectory
    QStringList searchPaths = {
        QCoreApplication::applicationDirPath(),
        QCoreApplication::applicationDirPath() + "/translations",
        ":/translations"
    };

    bool loaded = false;
    for (const QString &path : searchPaths) {
        if (m_translator.load(locale, path)) {
            m_app->installTranslator(&m_translator);
            loaded = true;
            break;
        }
    }

    if (!loaded && locale != "ru_RU") {
        qDebug() << "Translation not found for" << locale << "— falling back to default";
    }

    m_currentLocale = locale;
    SettingsManager::instance().setLanguage(locale);
}

void LanguageSwitcher::onLanguageActionTriggered()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action) return;
    QString locale = action->data().toString();
    if (locale == m_currentLocale) return;
    loadLanguage(locale);
    emit languageChanged(locale);
}
