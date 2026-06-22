#ifndef LANGUAGESWITCHER_H
#define LANGUAGESWITCHER_H

#include <QObject>
#include <QTranslator>
#include <QApplication>
#include <QMenu>
#include <QAction>

class LanguageSwitcher : public QObject
{
    Q_OBJECT
public:
    explicit LanguageSwitcher(QApplication *app, QObject *parent = nullptr);

    QMenu* createLanguageMenu(QWidget *menuParent);
    void loadLanguage(const QString &locale);
    QString currentLocale() const { return m_currentLocale; }

signals:
    void languageChanged(const QString &locale);

private slots:
    void onLanguageActionTriggered();

private:
    QApplication *m_app;
    QTranslator m_translator;
    QString m_currentLocale;
};

#endif // LANGUAGESWITCHER_H
