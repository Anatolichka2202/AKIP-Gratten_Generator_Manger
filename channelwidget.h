#ifndef CHANNELWIDGET_H
#define CHANNELWIDGET_H

#include <QGroupBox>
#include <QElapsedTimer>
#include "iakipcontroller.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ChannelWidget; }
QT_END_NAMESPACE

class ChannelWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit ChannelWidget(int channelNumber, IAkipController *controller, QWidget *parent = nullptr);
    ~ChannelWidget();
    void setController(IAkipController *controller);
    void setEnabled(bool enabled);

signals:
    void logMessage(const QString &msg);

private slots:
    void onSetFreqClicked();
    void onQueryFreqClicked();
    void onSetOutputClicked();
    void onQueryOutputClicked();
    void onSetAmplClicked();
    void onQueryAmplClicked();
    void onSetWaveformClicked();
    void onQueryWaveformClicked();

    void onFrequencyChanged(int channel, double freq);
    void onOutputChanged(int channel, bool enabled);
    void onAmplitudeChanged(int channel, double amplitude);
    void onWaveformChanged(int channel, const QString &waveform);

private:
    Ui::ChannelWidget *ui;
    IAkipController *m_controller;
    int m_channel;
    QElapsedTimer m_timer;
    void updateLastOpTime(qint64 ms);
    void connectControllerSignals();
    void disconnectControllerSignals();
};

#endif // CHANNELWIDGET_H
