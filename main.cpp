#include <QApplication>
#include "mainwindow.h"
#include "version.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName(APP_NAME);
    QCoreApplication::setOrganizationName(APP_ORG);
    QCoreApplication::setOrganizationDomain(APP_ORG_DOMAIN);
    QCoreApplication::setApplicationVersion(APP_VERSION_STRING);
    MainWindow window;
    window.setWindowTitle(QString("%1 v%2").arg(APP_NAME).arg(APP_VERSION_STRING));
    window.show();
    return app.exec();
}
