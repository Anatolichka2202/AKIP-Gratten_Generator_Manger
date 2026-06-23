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

    void setLastDevice(const QString &name, const QString &address);

signals:
    void connectRequested();
    void settingsRequested();

private:
    QLabel *m_lastDeviceLabel;
};
