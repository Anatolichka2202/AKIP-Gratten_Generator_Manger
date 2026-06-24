#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>
#include "mainwindow.h"
#include "version.h"
#include "thememanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName(APP_NAME);
    QCoreApplication::setOrganizationName(APP_ORG);
    QCoreApplication::setOrganizationDomain(APP_ORG_DOMAIN);
    QCoreApplication::setApplicationVersion(APP_VERSION_STRING);

    app.setWindowIcon(QIcon(":/icons/design/icons/app.ico"));

    QCommandLineParser parser;
    parser.addOption({"light", "Use light theme (default: dark)"});
    parser.process(app);

    ThemeManager::apply(app,
        parser.isSet("light") ? ThemeManager::Theme::Light : ThemeManager::Theme::Dark);

    MainWindow window;
    window.setWindowTitle(QString("%1 v%2").arg(APP_NAME).arg(APP_VERSION_STRING));
    window.show();
    return app.exec();
}
