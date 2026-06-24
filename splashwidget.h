#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QEvent>

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

protected:
    void changeEvent(QEvent *event) override;

private:
    void retranslateStrings();
    QLabel      *m_lastDeviceLabel;
    QLabel      *m_statusText;
    QLabel      *m_hint;
    QLabel      *m_devInfo;
    QPushButton *m_btnQuickConnect;
    QPushButton *m_btnOther;
    QPushButton *m_btnSettings;

    QString m_lastDeviceName;
    QString m_lastDeviceAddress;
    QString m_lastDeviceType;
};
