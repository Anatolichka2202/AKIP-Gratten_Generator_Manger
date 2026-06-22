#include "channelwidget.h"
#include "ui_channelwidget.h"

ChannelWidget::ChannelWidget(int channelNumber, IAkipController *controller, QWidget *parent)
    : QGroupBox(parent)
    , ui(new Ui::ChannelWidget)
    , m_controller(controller)
    , m_channel(channelNumber)
{
    ui->setupUi(this);

    setTitle(tr("Канал %1").arg(QChar('A' + channelNumber - 1)));

    // Connect button signals
    connect(ui->btnSetFreq, &QPushButton::clicked, this, &ChannelWidget::onSetFreqClicked);
    connect(ui->btnQueryFreq, &QPushButton::clicked, this, &ChannelWidget::onQueryFreqClicked);
    connect(ui->btnSetOutput, &QPushButton::clicked, this, &ChannelWidget::onSetOutputClicked);
    connect(ui->btnQueryOutput, &QPushButton::clicked, this, &ChannelWidget::onQueryOutputClicked);
    connect(ui->btnSetAmpl, &QPushButton::clicked, this, &ChannelWidget::onSetAmplClicked);
    connect(ui->btnQueryAmpl, &QPushButton::clicked, this, &ChannelWidget::onQueryAmplClicked);
    connect(ui->btnSetWave, &QPushButton::clicked, this, &ChannelWidget::onSetWaveformClicked);
    connect(ui->btnQueryWave, &QPushButton::clicked, this, &ChannelWidget::onQueryWaveformClicked);

    connectControllerSignals();
    setEnabled(false);
}

ChannelWidget::~ChannelWidget()
{
    delete ui;
}

void ChannelWidget::setController(IAkipController *controller)
{
    if (m_controller == controller)
        return;

    disconnectControllerSignals();
    m_controller = controller;
    connectControllerSignals();
}

void ChannelWidget::setEnabled(bool enabled)
{
    QWidget::setEnabled(enabled);
}

void ChannelWidget::connectControllerSignals()
{
    if (!m_controller)
        return;

    connect(m_controller, &IAkipController::frequencyChanged, this, &ChannelWidget::onFrequencyChanged);
    connect(m_controller, &IAkipController::outputChanged, this, &ChannelWidget::onOutputChanged);
    connect(m_controller, &IAkipController::amplitudeChanged, this, &ChannelWidget::onAmplitudeChanged);
    connect(m_controller, &IAkipController::waveformChanged, this, &ChannelWidget::onWaveformChanged);
}

void ChannelWidget::disconnectControllerSignals()
{
    if (!m_controller)
        return;

    disconnect(m_controller, &IAkipController::frequencyChanged, this, &ChannelWidget::onFrequencyChanged);
    disconnect(m_controller, &IAkipController::outputChanged, this, &ChannelWidget::onOutputChanged);
    disconnect(m_controller, &IAkipController::amplitudeChanged, this, &ChannelWidget::onAmplitudeChanged);
    disconnect(m_controller, &IAkipController::waveformChanged, this, &ChannelWidget::onWaveformChanged);
}

void ChannelWidget::updateLastOpTime(qint64 ms)
{
    ui->lblLastOpTime->setText(tr("Время операции: %1 мс").arg(ms));
}

void ChannelWidget::onSetFreqClicked()
{
    if (!m_controller || !m_controller->isAvailable()) {
        emit logMessage(tr("Контроллер недоступен"));
        return;
    }

    double freq = ui->editFreq->text().toDouble();
    if (freq <= 0) {
        emit logMessage(tr("Некорректная частота для канала %1").arg(m_channel));
        return;
    }

    m_timer.start();
    bool ok = m_controller->setFrequency(m_channel, freq);
    qint64 elapsed = m_timer.elapsed();

    if (ok) {
        updateLastOpTime(elapsed);
        emit logMessage(tr("Канал %1: частота установлена в %2 Гц за %3 мс")
                        .arg(m_channel).arg(freq).arg(elapsed));
    } else {
        emit logMessage(tr("Ошибка установки частоты канала %1").arg(m_channel));
    }
}

void ChannelWidget::onQueryFreqClicked()
{
    if (!m_controller || !m_controller->isAvailable()) {
        emit logMessage(tr("Контроллер недоступен"));
        return;
    }

    m_timer.start();
    double freq = m_controller->queryFrequency(m_channel);
    qint64 elapsed = m_timer.elapsed();

    if (freq > 0) {
        ui->editFreq->setText(QString::number(freq));
        updateLastOpTime(elapsed);
        emit logMessage(tr("Канал %1: запрос частоты, ответ: %2 Гц, время: %3 мс")
                        .arg(m_channel).arg(freq).arg(elapsed));
    } else {
        emit logMessage(tr("Ошибка запроса частоты канала %1").arg(m_channel));
    }
}

void ChannelWidget::onSetOutputClicked()
{
    if (!m_controller || !m_controller->isAvailable()) {
        emit logMessage(tr("Контроллер недоступен"));
        return;
    }

    bool enable = ui->chkOutput->isChecked();
    m_timer.start();
    bool ok = m_controller->setOutput(m_channel, enable);
    qint64 elapsed = m_timer.elapsed();

    if (ok) {
        updateLastOpTime(elapsed);
        emit logMessage(tr("Канал %1: выход %2 за %3 мс")
                        .arg(m_channel).arg(enable ? "ON" : "OFF").arg(elapsed));
    } else {
        emit logMessage(tr("Ошибка установки выхода канала %1").arg(m_channel));
    }
}

void ChannelWidget::onQueryOutputClicked()
{
    if (!m_controller || !m_controller->isAvailable()) {
        emit logMessage(tr("Контроллер недоступен"));
        return;
    }

    m_timer.start();
    bool on = m_controller->queryOutput(m_channel);
    qint64 elapsed = m_timer.elapsed();

    ui->chkOutput->setChecked(on);
    updateLastOpTime(elapsed);
    emit logMessage(tr("Канал %1: запрос выхода, ответ: %2, время: %3 мс")
                    .arg(m_channel).arg(on ? "ON" : "OFF").arg(elapsed));
}

void ChannelWidget::onSetAmplClicked()
{
    if (!m_controller || !m_controller->isAvailable()) {
        emit logMessage(tr("Контроллер недоступен"));
        return;
    }

    double ampl = ui->editAmpl->text().toDouble();
    if (ampl <= 0) {
        emit logMessage(tr("Некорректная амплитуда для канала %1").arg(m_channel));
        return;
    }

    QString unit = ui->cmbAmplUnit->currentText();
    m_timer.start();
    bool ok = m_controller->setAmplitude(m_channel, ampl, unit);
    qint64 elapsed = m_timer.elapsed();

    if (ok) {
        updateLastOpTime(elapsed);
        emit logMessage(tr("Канал %1: амплитуда установлена в %2 %3 за %4 мс")
                        .arg(m_channel).arg(ampl).arg(unit).arg(elapsed));
    } else {
        emit logMessage(tr("Ошибка установки амплитуды канала %1").arg(m_channel));
    }
}

void ChannelWidget::onQueryAmplClicked()
{
    if (!m_controller || !m_controller->isAvailable()) {
        emit logMessage(tr("Контроллер недоступен"));
        return;
    }

    m_timer.start();
    double ampl = m_controller->queryAmplitude(m_channel);
    qint64 elapsed = m_timer.elapsed();

    if (ampl > 0) {
        ui->editAmpl->setText(QString::number(ampl));
        updateLastOpTime(elapsed);
        emit logMessage(tr("Канал %1: запрос амплитуды, ответ: %2, время: %3 мс")
                        .arg(m_channel).arg(ampl).arg(elapsed));
    } else {
        emit logMessage(tr("Ошибка запроса амплитуды канала %1").arg(m_channel));
    }
}

void ChannelWidget::onSetWaveformClicked()
{
    if (!m_controller || !m_controller->isAvailable()) {
        emit logMessage(tr("Контроллер недоступен"));
        return;
    }

    QString wave = ui->cmbWaveform->currentText();
    m_timer.start();
    bool ok = m_controller->setWaveform(m_channel, wave);
    qint64 elapsed = m_timer.elapsed();

    if (ok) {
        updateLastOpTime(elapsed);
        emit logMessage(tr("Канал %1: форма сигнала %2 установлена за %3 мс")
                        .arg(m_channel).arg(wave).arg(elapsed));
    } else {
        emit logMessage(tr("Ошибка установки формы сигнала канала %1").arg(m_channel));
    }
}

void ChannelWidget::onQueryWaveformClicked()
{
    if (!m_controller || !m_controller->isAvailable()) {
        emit logMessage(tr("Контроллер недоступен"));
        return;
    }

    m_timer.start();
    QString wave = m_controller->queryWaveform(m_channel);
    qint64 elapsed = m_timer.elapsed();

    if (!wave.isEmpty()) {
        int idx = ui->cmbWaveform->findText(wave, Qt::MatchFixedString);
        if (idx >= 0)
            ui->cmbWaveform->setCurrentIndex(idx);
        updateLastOpTime(elapsed);
        emit logMessage(tr("Канал %1: запрос формы сигнала, ответ: %2, время: %3 мс")
                        .arg(m_channel).arg(wave).arg(elapsed));
    } else {
        emit logMessage(tr("Ошибка запроса формы сигнала канала %1").arg(m_channel));
    }
}

void ChannelWidget::onFrequencyChanged(int channel, double freq)
{
    if (channel == m_channel)
        ui->editFreq->setText(QString::number(freq));
}

void ChannelWidget::onOutputChanged(int channel, bool enabled)
{
    if (channel == m_channel)
        ui->chkOutput->setChecked(enabled);
}

void ChannelWidget::onAmplitudeChanged(int channel, double amplitude)
{
    if (channel == m_channel)
        ui->editAmpl->setText(QString::number(amplitude));
}

void ChannelWidget::onWaveformChanged(int channel, const QString &waveform)
{
    if (channel == m_channel) {
        int idx = ui->cmbWaveform->findText(waveform, Qt::MatchFixedString);
        if (idx >= 0)
            ui->cmbWaveform->setCurrentIndex(idx);
    }
}
