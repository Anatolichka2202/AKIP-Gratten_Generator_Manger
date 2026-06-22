#ifndef IAKIPCONTROLLER_H
#define IAKIPCONTROLLER_H

#include <QObject>
#include <QString>
#include <QStringList>

class IAkipController : public QObject
{
    Q_OBJECT

public:
    explicit IAkipController(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~IAkipController() {}

    // ==================== Управление подключением ====================
    virtual bool openDevice(int index = 0) = 0;
    virtual void closeDevice() = 0;
    virtual bool isOpen() const = 0;
    virtual bool isAvailable() const = 0;

    // ==================== Высокоуровневые команды ====================
    virtual bool setFrequency(int channel, double freqHz) = 0;
    virtual bool setOutput(int channel, bool enable) = 0;
    virtual bool setAmplitude(int channel, double amplitude, const QString &unit = "VPP") = 0;
    virtual bool setWaveform(int channel, const QString &waveform) = 0;
    virtual bool setDutyCycle(int channel, double percent) = 0;          // NEW
    virtual bool setAMFrequency(int channel, double freqHz) = 0;        // NEW
    virtual bool setAMDepth(int channel, double percent) = 0;           // NEW
    virtual bool setAMState(int channel, bool enable) = 0;              // NEW
    virtual QString getIdentity() = 0;
    virtual bool reset() = 0;

    // ==================== FM модуляция ====================
    virtual bool setFMFrequency(int channel, double freqHz) = 0;
    virtual bool setFMDeviation(int channel, double freqHz) = 0;
    virtual bool setFMState(int channel, bool enable) = 0;
    virtual double queryFMFrequency(int channel) = 0;
    virtual double queryFMDeviation(int channel) = 0;
    virtual bool queryFMState(int channel) = 0;

    // ==================== PM модуляция ====================
    virtual bool setPMFrequency(int channel, double freqHz) = 0;
    virtual bool setPMDeviation(int channel, double rad) = 0;
    virtual bool setPMState(int channel, bool enable) = 0;
    virtual double queryPMFrequency(int channel) = 0;
    virtual double queryPMDeviation(int channel) = 0;
    virtual bool queryPMState(int channel) = 0;

    // ==================== Sweep ====================
    virtual bool setSweepStart(int channel, double freqHz) = 0;
    virtual bool setSweepStop(int channel, double freqHz) = 0;
    virtual bool setSweepDwellTime(int channel, double seconds) = 0;
    virtual bool setSweepState(int channel, bool enable) = 0;
    virtual double querySweepStart(int channel) = 0;
    virtual double querySweepStop(int channel) = 0;
    virtual bool querySweepState(int channel) = 0;

    // ==================== PULM ====================
    virtual bool setPULMPeriod(int channel, double seconds) = 0;
    virtual bool setPULMWidth(int channel, double seconds) = 0;
    virtual bool setPULMState(int channel, bool enable) = 0;
    virtual double queryPULMPeriod(int channel) = 0;
    virtual double queryPULMWidth(int channel) = 0;
    virtual bool queryPULMState(int channel) = 0;

    // ==================== LF output ====================
    virtual bool setLFFrequency(int channel, double freqHz) = 0;
    virtual bool setLFAmplitude(int channel, double volts) = 0;
    virtual bool setLFState(int channel, bool enable) = 0;
    virtual double queryLFFrequency(int channel) = 0;
    virtual double queryLFAmplitude(int channel) = 0;
    virtual bool queryLFState(int channel) = 0;
    // ==================== Запросы текущих значений ====================
    virtual double queryFrequency(int channel) = 0;
    virtual bool queryOutput(int channel) = 0;
    virtual double queryAmplitude(int channel) = 0;
    virtual QString queryWaveform(int channel) = 0;
    virtual double queryDutyCycle(int channel) = 0;                      // NEW
    virtual double queryAMFrequency(int channel) = 0;                    // NEW
    virtual double queryAMDepth(int channel) = 0;                        // NEW
    virtual bool queryAMState(int channel) = 0;                          // NEW

    // ==================== Низкоуровневые команды ====================
    virtual bool sendCommand(const QString &cmd) = 0;
    virtual QString queryCommand(const QString &cmd) = 0;

    // ==================== Список поддерживаемых команд ====================
    virtual QStringList availableCommands() const = 0;

    // ==================== Методы с замером времени ====================
    virtual bool sendCommandTimed(const QString &cmd, qint64 &elapsedMs) = 0;
    virtual bool queryCommandTimed(const QString &cmd, QString &response, qint64 &elapsedMs) = 0;

signals:
    void opened();
    void closed();
    void availabilityChanged(bool available);
    void errorOccurred(const QString &error);
    void frequencyChanged(int channel, double freq);
    void outputChanged(int channel, bool enabled);
    void amplitudeChanged(int channel, double amplitude);
    void waveformChanged(int channel, const QString &waveform);
    void dutyCycleChanged(int channel, double percent);                  // NEW
    void amFrequencyChanged(int channel, double freq);                   // NEW
    void amDepthChanged(int channel, double percent);                    // NEW
    void amStateChanged(int channel, bool enabled);                      // NEW

    // FM signals
    void fmFrequencyChanged(int channel, double freq);
    void fmDeviationChanged(int channel, double deviation);
    void fmStateChanged(int channel, bool enabled);

    // PM modulation signals
    void pmFrequencyChanged(int channel, double freq);
    void pmDeviationChanged(int channel, double deviation);
    void pmStateChanged(int channel, bool enabled);
    // Sweep signals
    void sweepStartChanged(int channel, double freq);
    void sweepStopChanged(int channel, double freq);
    void sweepStateChanged(int channel, bool enabled);

    // PULM signals
    void pulmPeriodChanged(int channel, double period);
    void pulmWidthChanged(int channel, double width);
    void pulmStateChanged(int channel, bool enabled);

    // LF output signals
    void lfFrequencyChanged(int channel, double freq);
    void lfAmplitudeChanged(int channel, double amplitude);
    void lfStateChanged(int channel, bool enabled);
};

#endif // IAKIPCONTROLLER_H
