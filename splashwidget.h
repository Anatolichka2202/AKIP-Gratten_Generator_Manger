#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

class SplashWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SplashWidget(QWidget *parent = nullptr);

    // Call with non-empty args to show quick-reconnect button.
    // deviceType: "AKIP" | "GRATTEN" | "" (clears quick-connect)
    void setLastDevice(const QString &name, const QString &address,
                       const QString &deviceType = QString());

signals:
    void connectRequested();           // open device selection dialog
    void quickConnectRequested(const QString &deviceType); // direct reconnect
    void settingsRequested();

private:
    QLabel      *m_lastDeviceLabel;
    QPushButton *m_btnQuickConnect;
    QPushButton *m_btnOther;
};
