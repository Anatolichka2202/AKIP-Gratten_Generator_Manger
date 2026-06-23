#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QElapsedTimer>
#include <QLabel>
#include "iakipcontroller.h"
#include "akipfacade.h"
#include "grattenga1483controller.h"
#include "grattencontrolwidget.h"
#include "settingsdialog.h"
#include "languageswitcher.h"
#include "channelwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class LanguageSwitcher;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Подключение
    void on_btnConnect_clicked();
    void on_btnDisconnect_clicked();
    void on_btnIdn_clicked();

    // Тестирование задержек
    void on_btnTestIdn_clicked();
    void on_btnTestSetFreq_clicked();
    void on_btnTestQueryFreq_clicked();
    void on_btnTestSeriesIdn_clicked();

    // Лог
    void on_btnLogClear_clicked();
    void on_btnLogSave_clicked();

    // Слоты от контроллера
    void onControllerAvailabilityChanged(bool available);
    void onControllerError(const QString &error);
    void onChannelWidgetLogMessage(const QString &msg);

private:
    void logMessage(const QString &msg);
    void setChannelControlsEnabled(bool enabled);
    void updateConnectionStatus();
    void updateStatusBar();

    enum DeviceType { Unknown, AKIP, GRATTEN };
    IAkipController *m_controller;
    DeviceType m_currentType;
    void checkAvailableDevices();
    void switchToDevice(DeviceType type);
    void showDeviceSelectionDialog(const QString &akipIdn, const QString &grattenIdn);

    Ui::MainWindow *ui;
    QElapsedTimer m_timer;

    QWidget *m_akipPage;
    GrattenControlWidget *m_grattenPage;
    ChannelWidget *m_channelA;
    ChannelWidget *m_channelB;
    LanguageSwitcher *m_langSwitcher;

    QLabel *m_sbDevice;
    QLabel *m_sbConn;
    QLabel *m_sbLastOp;

    void setupForDeviceType(DeviceType type);
    void setupMenu();
};

#endif // MAINWINDOW_H
