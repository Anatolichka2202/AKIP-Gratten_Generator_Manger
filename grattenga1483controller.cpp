#include "grattenga1483controller.h"
#include <QThread>
#include "gratten_limits.h"
GrattenGa1483Controller::GrattenGa1483Controller(QObject *parent)
    : AbstractScpiController(parent)
    , m_available(false)
    , m_port(5025)
{
    connect(&m_lan, &LanInterface::connected, this, &GrattenGa1483Controller::onConnected);
    connect(&m_lan, &LanInterface::disconnected, this, &GrattenGa1483Controller::onDisconnected);
    connect(&m_lan, &LanInterface::errorOccurred, this, &GrattenGa1483Controller::onError);

        // Регистрация команд из конфигурации
    for (const auto &info : GRATTEN_COMMANDS) {
        AbstractScpiController::CommandDef def;
        def.setPattern = info.setPattern;
        def.queryCmd = info.queryCmd;
        def.minVal = info.minVal;
        def.maxVal = info.maxVal;
        def.unit = info.unit;
        def.isBoolean = info.isBoolean;
        registerCommand(info.key, def);
    }
}

GrattenGa1483Controller::~GrattenGa1483Controller()
{
    closeDevice();
}

void GrattenGa1483Controller::setConnectionParameters(const QString &host, quint16 port)
{
    m_host = host;
    m_port = port;
}

bool GrattenGa1483Controller::openDevice(int /*index*/)
{
    if (m_host.isEmpty()) {
        emit errorOccurred("Host not set. Call setConnectionParameters() first.");
        return false;
    }

    if (m_lan.isConnected())
        closeDevice();

    if (!m_lan.connectToHost(m_host, m_port)) {
        m_available = false;
        return false;
    }

    // После подключения проверяем доступность через *IDN?
    QString idn = queryCommand("*IDN?");
    m_available = !idn.isEmpty();
    if (m_available) {
        emit opened();
        emit availabilityChanged(true);
    } else {
        emit errorOccurred("Device opened but does not respond to *IDN?");
    }
    return m_available;
}

void GrattenGa1483Controller::closeDevice()
{
    m_lan.disconnect();
    m_available = false;
    emit closed();
    emit availabilityChanged(false);
}

bool GrattenGa1483Controller::isOpen() const
{
    return m_lan.isConnected();
}

bool GrattenGa1483Controller::isAvailable() const
{
    return m_available && m_lan.isConnected();
}

bool GrattenGa1483Controller::isTransportAvailable() const
{
    return m_lan.isConnected();
}

QString GrattenGa1483Controller::querySystemError()
{
    if (!isAvailable()) return QString();
    // Используем малый таймаут и без повторов, чтобы не засорять лог
    QString resp = m_lan.queryScpiCommand(":SYSTem:ERRor?", 300, 0);
    if (resp.isEmpty()) {
        // Команда не поддерживается или нет ошибок — возвращаем "0"
        return "0";
    }
    return resp;
}

// ==================== Высокоуровневые команды ====================

bool GrattenGa1483Controller::setFrequency(int channel, double freqHz)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;

    if (!checkRange("FREQ", freqHz))
        return false;

    QString cmd = buildSetCommand("FREQ", freqHz);
    return verifySetCommand(cmd,
                            [this, channel]() { return queryFrequency(channel); },
                            freqHz, 1.0);
}

bool GrattenGa1483Controller::setOutput(int channel, bool enable)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;

    QString cmd = buildSetCommand("OUTP", enable ? 1.0 : 0.0);
    return verifySetCommand(cmd,
                            [this, channel]() { return queryOutput(channel); },
                            enable);
}

bool GrattenGa1483Controller::setAmplitude(int channel, double amplitude, const QString &unit)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;

    if (unit.toUpper() != "DBM") {
        emit errorOccurred("Only dBm unit supported for RF amplitude on Gratten");
        return false;
    }

    if (!checkRange("AMPL", amplitude))
        return false;

    QString cmd = buildSetCommand("AMPL", amplitude);
    return verifySetCommand(cmd,
                            [this, channel]() { return queryAmplitude(channel); },
                            amplitude, 0.1);
}

bool GrattenGa1483Controller::setWaveform(int channel, const QString &waveform)
{
    Q_UNUSED(channel);
    Q_UNUSED(waveform);
    if (!ensureAvailable()) return false;
    emit errorOccurred("Waveform selection is not supported for GA1483/GA1484");
    return false;
}

bool GrattenGa1483Controller::setDutyCycle(int channel, double percent)
{
    Q_UNUSED(channel);
    Q_UNUSED(percent);
    if (!ensureAvailable()) return false;
    emit errorOccurred("Duty cycle is not supported for GA1483/GA1484");
    return false;
}

bool GrattenGa1483Controller::setAMFrequency(int channel, double freqHz)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;

    if (!checkRange("AMFREQ", freqHz))
        return false;

    QString cmd = buildSetCommand("AMFREQ", freqHz);
    return verifySetCommand(cmd,
                            [this, channel]() { return queryAMFrequency(channel); },
                            freqHz, 1.0);
}

bool GrattenGa1483Controller::setAMDepth(int channel, double percent)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;

    if (!checkRange("AMDEPTH", percent))
        return false;

    QString cmd = buildSetCommand("AMDEPTH", percent);
    return verifySetCommand(cmd,
                            [this, channel]() { return queryAMDepth(channel); },
                            percent, 0.1);
}

bool GrattenGa1483Controller::setAMState(int channel, bool enable)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;

    QString cmd = buildSetCommand("AMSTATE", enable ? 1.0 : 0.0);
    return verifySetCommand(cmd,
                            [this, channel]() { return queryAMState(channel); },
                            enable);
}

QString GrattenGa1483Controller::getIdentity()
{
    if (!ensureAvailable()) return QString();
    return queryCommand("*IDN?");
}

bool GrattenGa1483Controller::reset()
{
    if (!ensureAvailable()) return false;
    // *RST не возвращает ответа, поэтому используем sendCommand
    bool ok = sendCommand("*RST");
    // Даём прибору время на перезагрузку (опционально)
    if (ok) QThread::msleep(100);
    return ok;
}

// ==================== Запросы текущих значений ====================

double GrattenGa1483Controller::queryFrequency(int channel)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return 0.0;
    QString resp = queryCommand(":FREQuency:CW?");
    bool ok;
    double val = resp.toDouble(&ok);
    if (ok) {
        m_freqCache[channel] = val;
        return val;
    }
    return 0.0;
}

bool GrattenGa1483Controller::queryOutput(int channel)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    QString resp = queryCommand(":OUTPut:STATE?");
    if (resp.contains("ON", Qt::CaseInsensitive) || resp.contains("1"))
        return true;
    return false;
}

double GrattenGa1483Controller::queryAmplitude(int channel)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return 0.0;
    QString resp = queryCommand(":POWer:LEVEL?");
    bool ok;
    double val = resp.toDouble(&ok);
    if (ok) {
        m_amplCache[channel] = val;
        return val;
    }
    return 0.0;
}

QString GrattenGa1483Controller::queryWaveform(int channel)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return QString();
    QString resp = queryCommand(":FUNCtion:SHAPe?");
    if (!resp.isEmpty()) {
        m_waveCache[channel] = resp;
        return resp;
    }
    return QString();
}

double GrattenGa1483Controller::queryDutyCycle(int channel)
{
    Q_UNUSED(channel);
    return 0.0;
}

double GrattenGa1483Controller::queryAMFrequency(int channel)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return 0.0;
    QString resp = queryCommand(":AM:INTernal:FREQuency?");
    bool ok;
    return resp.toDouble(&ok) ? resp.toDouble() : 0.0;
}

double GrattenGa1483Controller::queryAMDepth(int channel)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return 0.0;
    QString resp = queryCommand(":AM:DEPTh?");
    bool ok;
    return resp.toDouble(&ok) ? resp.toDouble() : 0.0;
}

bool GrattenGa1483Controller::queryAMState(int channel)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    QString resp = queryCommand(":AM:STATE?");
    return resp.contains("ON", Qt::CaseInsensitive) || resp.contains("1");
}

// ==================== FM модуляция ====================

bool GrattenGa1483Controller::setFMFrequency(int channel, double freqHz)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    if (!checkRange("FMFREQ", freqHz)) return false;
    QString cmd = buildSetCommand("FMFREQ", freqHz);
    return verifySetCommand(cmd,
                            [this, channel]() { return queryFMFrequency(channel); },
                            freqHz, 1.0);
}

bool GrattenGa1483Controller::setFMDeviation(int channel, double freqHz)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    if (!checkRange("FMDEV", freqHz)) return false;
    QString cmd = buildSetCommand("FMDEV", freqHz);
    return verifySetCommand(cmd,
                            [this, channel]() { return queryFMDeviation(channel); },
                            freqHz, 1.0);
}

bool GrattenGa1483Controller::setFMState(int channel, bool enable)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    QString cmd = buildSetCommand("FMSTATE", enable ? 1.0 : 0.0);
    bool ok = verifySetCommand(cmd,
                               [this, channel]() { return queryFMState(channel); },
                               enable);
    if (ok) emit fmStateChanged(channel, enable);
    return ok;
}

double GrattenGa1483Controller::queryFMFrequency(int channel)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return 0.0;
    QString resp = queryCommand(buildQueryCommand("FMFREQ"));
    bool ok;
    double val = resp.toDouble(&ok);
    if (ok) { m_fmFreqCache[channel] = val; return val; }
    return 0.0;
}

double GrattenGa1483Controller::queryFMDeviation(int channel)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return 0.0;
    QString resp = queryCommand(buildQueryCommand("FMDEV"));
    bool ok;
    double val = resp.toDouble(&ok);
    if (ok) { m_fmDevCache[channel] = val; return val; }
    return 0.0;
}

bool GrattenGa1483Controller::queryFMState(int channel)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    QString resp = queryCommand(buildQueryCommand("FMSTATE"));
    bool on = resp.contains("ON", Qt::CaseInsensitive) || resp.contains("1");
    m_fmStateCache[channel] = on;
    return on;
}

// ==================== PM модуляция ====================

bool GrattenGa1483Controller::setPMFrequency(int channel, double freqHz)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    if (!checkRange("PMFREQ", freqHz)) return false;
    QString cmd = buildSetCommand("PMFREQ", freqHz);
    return verifySetCommand(cmd,
                            [this, channel]() { return queryPMFrequency(channel); },
                            freqHz, 1.0);
}

bool GrattenGa1483Controller::setPMDeviation(int channel, double rad)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    if (!checkRange("PMDEV", rad)) return false;
    QString cmd = buildSetCommand("PMDEV", rad);
    return verifySetCommand(cmd,
                            [this, channel]() { return queryPMDeviation(channel); },
                            rad, 0.1);
}

bool GrattenGa1483Controller::setPMState(int channel, bool enable)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    QString cmd = buildSetCommand("PMSTATE", enable ? 1.0 : 0.0);
    bool ok = verifySetCommand(cmd,
                               [this, channel]() { return queryPMState(channel); },
                               enable);
    if (ok) emit pmStateChanged(channel, enable);
    return ok;
}

double GrattenGa1483Controller::queryPMFrequency(int channel)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return 0.0;
    QString resp = queryCommand(buildQueryCommand("PMFREQ"));
    bool ok;
    double val = resp.toDouble(&ok);
    if (ok) { m_pmFreqCache[channel] = val; return val; }
    return 0.0;
}

double GrattenGa1483Controller::queryPMDeviation(int channel)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return 0.0;
    QString resp = queryCommand(buildQueryCommand("PMDEV"));
    bool ok;
    double val = resp.toDouble(&ok);
    if (ok) { m_pmDevCache[channel] = val; return val; }
    return 0.0;
}

bool GrattenGa1483Controller::queryPMState(int channel)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    QString resp = queryCommand(buildQueryCommand("PMSTATE"));
    bool on = resp.contains("ON", Qt::CaseInsensitive) || resp.contains("1");
    m_pmStateCache[channel] = on;
    return on;
}

// ==================== Sweep ====================

bool GrattenGa1483Controller::setSweepStart(int channel, double freqHz)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    if (!checkRange("SWPSTART", freqHz)) return false;
    QString cmd = buildSetCommand("SWPSTART", freqHz);
    return verifySetCommand(cmd,
                            [this, channel]() { return querySweepStart(channel); },
                            freqHz, 1.0);
}

bool GrattenGa1483Controller::setSweepStop(int channel, double freqHz)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    if (!checkRange("SWPSTOP", freqHz)) return false;
    QString cmd = buildSetCommand("SWPSTOP", freqHz);
    return verifySetCommand(cmd,
                            [this, channel]() { return querySweepStop(channel); },
                            freqHz, 1.0);
}

bool GrattenGa1483Controller::setSweepDwellTime(int channel, double seconds)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    if (!checkRange("SWPDWELL", seconds)) return false;
    QString cmd = buildSetCommand("SWPDWELL", seconds);
    return verifySetCommand(cmd,
                            [this, channel]() { return querySweepStop(channel); },
                            seconds, 0.001);
}

bool GrattenGa1483Controller::setSweepState(int channel, bool enable)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    QString cmd = buildSetCommand("SWPSTATE", enable ? 1.0 : 0.0);
    bool ok = verifySetCommand(cmd,
                               [this, channel]() { return querySweepState(channel); },
                               enable);
    if (ok) emit sweepStateChanged(channel, enable);
    return ok;
}

double GrattenGa1483Controller::querySweepStart(int channel)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return 0.0;
    QString resp = queryCommand(buildQueryCommand("SWPSTART"));
    bool ok;
    double val = resp.toDouble(&ok);
    if (ok) { m_swpStartCache[channel] = val; return val; }
    return 0.0;
}

double GrattenGa1483Controller::querySweepStop(int channel)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return 0.0;
    QString resp = queryCommand(buildQueryCommand("SWPSTOP"));
    bool ok;
    double val = resp.toDouble(&ok);
    if (ok) { m_swpStopCache[channel] = val; return val; }
    return 0.0;
}

bool GrattenGa1483Controller::querySweepState(int channel)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    QString resp = queryCommand(buildQueryCommand("SWPSTATE"));
    bool on = resp.contains("ON", Qt::CaseInsensitive) || resp.contains("1");
    m_swpStateCache[channel] = on;
    return on;
}

// ==================== PULM ====================

bool GrattenGa1483Controller::setPULMPeriod(int channel, double seconds)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    if (!checkRange("PULMPERIOD", seconds)) return false;
    QString cmd = buildSetCommand("PULMPERIOD", seconds);
    return verifySetCommand(cmd,
                            [this, channel]() { return queryPULMPeriod(channel); },
                            seconds, 0.001);
}

bool GrattenGa1483Controller::setPULMWidth(int channel, double seconds)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    if (!checkRange("PULMWIDTH", seconds)) return false;
    QString cmd = buildSetCommand("PULMWIDTH", seconds);
    return verifySetCommand(cmd,
                            [this, channel]() { return queryPULMWidth(channel); },
                            seconds, 0.001);
}

bool GrattenGa1483Controller::setPULMState(int channel, bool enable)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    QString cmd = buildSetCommand("PULMSTATE", enable ? 1.0 : 0.0);
    bool ok = verifySetCommand(cmd,
                               [this, channel]() { return queryPULMState(channel); },
                               enable);
    if (ok) emit pulmStateChanged(channel, enable);
    return ok;
}

double GrattenGa1483Controller::queryPULMPeriod(int channel)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return 0.0;
    QString resp = queryCommand(buildQueryCommand("PULMPERIOD"));
    bool ok;
    double val = resp.toDouble(&ok);
    if (ok) { m_pulmPeriodCache[channel] = val; return val; }
    return 0.0;
}

double GrattenGa1483Controller::queryPULMWidth(int channel)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return 0.0;
    QString resp = queryCommand(buildQueryCommand("PULMWIDTH"));
    bool ok;
    double val = resp.toDouble(&ok);
    if (ok) { m_pulmWidthCache[channel] = val; return val; }
    return 0.0;
}

bool GrattenGa1483Controller::queryPULMState(int channel)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    QString resp = queryCommand(buildQueryCommand("PULMSTATE"));
    bool on = resp.contains("ON", Qt::CaseInsensitive) || resp.contains("1");
    m_pulmStateCache[channel] = on;
    return on;
}

// ==================== LF output ====================

bool GrattenGa1483Controller::setLFFrequency(int channel, double freqHz)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    if (!checkRange("LFFREQ", freqHz)) return false;
    QString cmd = buildSetCommand("LFFREQ", freqHz);
    return verifySetCommand(cmd,
                            [this, channel]() { return queryLFFrequency(channel); },
                            freqHz, 1.0);
}

bool GrattenGa1483Controller::setLFAmplitude(int channel, double volts)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    if (!checkRange("LFAMPL", volts)) return false;
    QString cmd = buildSetCommand("LFAMPL", volts);
    return verifySetCommand(cmd,
                            [this, channel]() { return queryLFAmplitude(channel); },
                            volts, 0.01);
}

bool GrattenGa1483Controller::setLFState(int channel, bool enable)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    QString cmd = buildSetCommand("LFSTATE", enable ? 1.0 : 0.0);
    bool ok = verifySetCommand(cmd,
                               [this, channel]() { return queryLFState(channel); },
                               enable);
    if (ok) emit lfStateChanged(channel, enable);
    return ok;
}

double GrattenGa1483Controller::queryLFFrequency(int channel)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return 0.0;
    QString resp = queryCommand(buildQueryCommand("LFFREQ"));
    bool ok;
    double val = resp.toDouble(&ok);
    if (ok) { m_lfFreqCache[channel] = val; return val; }
    return 0.0;
}

double GrattenGa1483Controller::queryLFAmplitude(int channel)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return 0.0;
    QString resp = queryCommand(buildQueryCommand("LFAMPL"));
    bool ok;
    double val = resp.toDouble(&ok);
    if (ok) { m_lfAmplCache[channel] = val; return val; }
    return 0.0;
}

bool GrattenGa1483Controller::queryLFState(int channel)
{
    Q_UNUSED(channel);
    if (!ensureAvailable()) return false;
    QString resp = queryCommand(buildQueryCommand("LFSTATE"));
    bool on = resp.contains("ON", Qt::CaseInsensitive) || resp.contains("1");
    m_lfStateCache[channel] = on;
    return on;
}

// ==================== Низкоуровневые команды ====================

bool GrattenGa1483Controller::sendCommand(const QString &cmd)
{
    if (!ensureAvailable()) return false;
    return m_lan.sendScpiCommand(cmd);
}

QString GrattenGa1483Controller::queryCommand(const QString &cmd)
{
    if (!ensureAvailable()) return QString();
    return m_lan.queryScpiCommand(cmd);
}

QStringList GrattenGa1483Controller::availableCommands() const
{
    return {
        "*IDN?", "*RST",
        ":FREQuency:CW", ":FREQuency:CW?",
        ":OUTPut:STATE", ":OUTPut:STATE?",
        ":POWer:LEVEL", ":POWer:LEVEL?",
        ":FUNCtion:SHAPe", ":FUNCtion:SHAPe?",
        ":AM:INTernal:FREQuency", ":AM:INTernal:FREQuency?",
        ":AM:DEPTh", ":AM:DEPTh?",
        ":AM:STATE", ":AM:STATE?",
        ":FM:STATe", ":FM:STATe?",
        ":FM:DEViation", ":FM:DEViation?",
        ":FM:INTernal:FREQuency", ":FM:INTernal:FREQuency?",
        ":PM:STATe", ":PM:STATe?",
        ":PM:DEViation", ":PM:DEViation?",
        ":PM:INTernal:FREQuency", ":PM:INTernal:FREQuency?",
        ":SWEep:STATe", ":SWEep:STATe?",
        ":SWEep:FREQuency:STARt", ":SWEep:FREQuency:STARt?",
        ":SWEep:FREQuency:STOP", ":SWEep:FREQuency:STOP?",
        ":SWEep:DWELl", ":SWEep:DWELl?",
        ":PULM:STATe", ":PULM:STATe?",
        ":PULM:INTernal:PERiod", ":PULM:INTernal:PERiod?",
        ":PULM:INTernal:WIDTh", ":PULM:INTernal:WIDTh?",
        ":LFOutput:STATe", ":LFOutput:STATe?",
        ":LFOutput:FREQuency", ":LFOutput:FREQuency?",
        ":LFOutput:AMPLitude", ":LFOutput:AMPLitude?"
    };
}

// ==================== Слоты ====================

void GrattenGa1483Controller::onConnected()
{
    // Уже обработано в openDevice
}

void GrattenGa1483Controller::onDisconnected()
{
    m_available = false;
    emit availabilityChanged(false);
}

void GrattenGa1483Controller::onError(const QString &error)
{
    emit errorOccurred(error);
}

QString GrattenGa1483Controller::unitToGratten(const QString &unit) const
{
    // Этот метод больше не используется, но оставим для совместимости
    QString upper = unit.toUpper();
    if (upper == "VPP" || upper == "V")
        return "V";
    if (upper == "DBM")
        return "DBM";
    return upper;
}
