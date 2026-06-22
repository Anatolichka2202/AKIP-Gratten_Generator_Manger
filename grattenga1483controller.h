#ifndef GRATTENGA1483CONTROLLER_H
#define GRATTENGA1483CONTROLLER_H

#include "abstractscpicontroller.h"
#include "laninterface.h"

class GrattenGa1483Controller : public AbstractScpiController
{
    Q_OBJECT
public:
    explicit GrattenGa1483Controller(QObject *parent = nullptr);
    ~GrattenGa1483Controller() override;

    // Дополнительный метод для установки параметров подключения (вызывать перед openDevice)
    void setConnectionParameters(const QString &host, quint16 port = 5025);

    // ==================== Управление подключением ====================
    bool openDevice(int index = 0) override;   // index игнорируется
    void closeDevice() override;
    bool isOpen() const override;
    bool isAvailable() const override;

    // ==================== Высокоуровневые команды ====================
    bool setFrequency(int channel, double freqHz) override;
    bool setOutput(int channel, bool enable) override;
    bool setAmplitude(int channel, double amplitude, const QString &unit = "VPP") override;
    bool setWaveform(int channel, const QString &waveform) override;
    bool setDutyCycle(int channel, double percent) override;
    bool setAMFrequency(int channel, double freqHz) override;
    bool setAMDepth(int channel, double percent) override;
    bool setAMState(int channel, bool enable) override;
    QString getIdentity() override;
    bool reset() override;

    // ==================== FM модуляция ====================
    bool setFMFrequency(int channel, double freqHz) override;
    bool setFMDeviation(int channel, double freqHz) override;
    bool setFMState(int channel, bool enable) override;
    double queryFMFrequency(int channel) override;
    double queryFMDeviation(int channel) override;
    bool queryFMState(int channel) override;

    // ==================== PM модуляция ====================
    bool setPMFrequency(int channel, double freqHz) override;
    bool setPMDeviation(int channel, double rad) override;
    bool setPMState(int channel, bool enable) override;
    double queryPMFrequency(int channel) override;
    double queryPMDeviation(int channel) override;
    bool queryPMState(int channel) override;

    // ==================== Sweep ====================
    bool setSweepStart(int channel, double freqHz) override;
    bool setSweepStop(int channel, double freqHz) override;
    bool setSweepDwellTime(int channel, double seconds) override;
    bool setSweepState(int channel, bool enable) override;
    double querySweepStart(int channel) override;
    double querySweepStop(int channel) override;
    bool querySweepState(int channel) override;

    // ==================== PULM ====================
    bool setPULMPeriod(int channel, double seconds) override;
    bool setPULMWidth(int channel, double seconds) override;
    bool setPULMState(int channel, bool enable) override;
    double queryPULMPeriod(int channel) override;
    double queryPULMWidth(int channel) override;
    bool queryPULMState(int channel) override;

    // ==================== LF output ====================
    bool setLFFrequency(int channel, double freqHz) override;
    bool setLFAmplitude(int channel, double volts) override;
    bool setLFState(int channel, bool enable) override;
    double queryLFFrequency(int channel) override;
    double queryLFAmplitude(int channel) override;
    bool queryLFState(int channel) override;
    // ==================== Запросы текущих значений ====================
    double queryFrequency(int channel) override;
    bool queryOutput(int channel) override;
    double queryAmplitude(int channel) override;
    QString queryWaveform(int channel) override;
    double queryDutyCycle(int channel) override;
    double queryAMFrequency(int channel) override;
    double queryAMDepth(int channel) override;
    bool queryAMState(int channel) override;

    // ==================== Низкоуровневые команды ====================
    bool sendCommand(const QString &cmd) override;
    QString queryCommand(const QString &cmd) override;

    // ==================== Список поддерживаемых команд ====================
    QStringList availableCommands() const override;

    // ==================== Чисто виртуальные методы AbstractScpiController ===============
    bool isTransportAvailable() const override;
    QString querySystemError() override;

private slots:
    void onConnected();
    void onDisconnected();
    void onError(const QString &error);

private:
    LanInterface m_lan;
    bool m_available;        // флаг, что устройство отвечает (после *IDN?)
    QString m_host;
    quint16 m_port;

    // Вспомогательные методы
    QString unitToGratten(const QString &unit) const; // перевод единиц (сохранён для совместимости, но не используется)
};

#endif // GRATTENGA1483CONTROLLER_H
