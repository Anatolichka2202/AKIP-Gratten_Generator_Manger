#include "sweepdialog.h"
#include "ui_sweepdialog.h"
#include <QDateTime>

SweepDialog::SweepDialog(IAkipController *controller, int channel, QWidget *parent)
    : QDialog(parent), ui(new Ui::SweepDialog), m_controller(controller), m_channel(channel)
{
    ui->setupUi(this);

    // Connect signals
    connect(ui->btnApply, &QPushButton::clicked, this, &SweepDialog::onApply);
    connect(ui->btnStop, &QPushButton::clicked, this, &SweepDialog::onStop);
    connect(ui->btnQuery, &QPushButton::clicked, this, &SweepDialog::onQuery);

    appendLog(tr("Диалог открыт"));
}

SweepDialog::~SweepDialog()
{
    delete ui;
}

void SweepDialog::onApply()
{
    if (!m_controller) {
        appendLog(tr("Ошибка: контроллер не инициализирован"));
        return;
    }

    appendLog(tr("Применение параметров свипа..."));

    // Set sweep parameters
    bool success = true;
    success &= m_controller->setSweepStart(m_channel, ui->spinStart->value());
    success &= m_controller->setSweepStop(m_channel, ui->spinStop->value());
    success &= m_controller->setSweepDwellTime(m_channel, ui->spinDwell->value());
    success &= m_controller->setSweepState(m_channel, true);

    if (success) {
        appendLog(tr("Свип запущен успешно"));
    } else {
        appendLog(tr("Ошибка при запуске свипа"));
    }
}

void SweepDialog::onStop()
{
    if (!m_controller) {
        appendLog(tr("Ошибка: контроллер не инициализирован"));
        return;
    }

    appendLog(tr("Остановка свипа..."));

    if (m_controller->setSweepState(m_channel, false)) {
        appendLog(tr("Свип остановлен"));
    } else {
        appendLog(tr("Ошибка при остановке свипа"));
    }
}

void SweepDialog::onQuery()
{
    if (!m_controller) {
        appendLog(tr("Ошибка: контроллер не инициализирован"));
        return;
    }

    appendLog(tr("Считывание параметров свипа..."));

    double startFreq = m_controller->querySweepStart(m_channel);
    double stopFreq = m_controller->querySweepStop(m_channel);
    bool state = m_controller->querySweepState(m_channel);

    ui->spinStart->setValue(startFreq);
    ui->spinStop->setValue(stopFreq);

    appendLog(tr("Нач. частота: %1 Гц").arg(startFreq, 0, 'f', 0));
    appendLog(tr("Кон. частота: %1 Гц").arg(stopFreq, 0, 'f', 0));
    appendLog(tr("Состояние: %1").arg(state ? tr("Включен") : tr("Отключен")));
}

void SweepDialog::appendLog(const QString &msg)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->txtLog->appendPlainText(QString("[%1] %2").arg(timestamp, msg));
}
