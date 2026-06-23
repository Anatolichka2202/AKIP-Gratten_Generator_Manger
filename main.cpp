#include <QApplication>
#include <QCommandLineParser>
#include "mainwindow.h"
#include "thememanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("AKIP/Gratten Generator Manager");
    QCoreApplication::setOrganizationName("TestLab");
    QCoreApplication::setApplicationVersion("1.0.0");

    QCommandLineParser parser;
    parser.addOption({"light", "Use light theme (default: dark)"});
    parser.process(app);

    ThemeManager::apply(app,
        parser.isSet("light") ? ThemeManager::Theme::Light : ThemeManager::Theme::Dark);

    MainWindow window;
    window.show();

    return app.exec();
}
