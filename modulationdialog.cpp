#include "modulationdialog.h"
#include "ui_modulationdialog.h"
#include <QDateTime>

ModulationDialog::ModulationDialog(IAkipController *controller, int channel, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ModulationDialog)
    , m_controller(controller)
    , m_channel(channel)
{
    ui->setupUi(this);
    setWindowTitle(tr("Настройка модуляции — канал %1").arg(channel));

    connect(ui->btnApplyAM,  &QPushButton::clicked, this, &ModulationDialog::onApplyAM);
    connect(ui->btnApplyFM,  &QPushButton::clicked, this, &ModulationDialog::onApplyFM);
    connect(ui->btnQueryAll, &QPushButton::clicked, this, &ModulationDialog::onQueryAll);
    connect(ui->buttonBox,   &QDialogButtonBox::rejected, this, &QDialog::reject);

    if (m_controller) {
        connect(m_controller, &IAkipController::errorOccurred,
                this, [this](const QString &err) { appendLog(tr("ОШИБКА: %1").arg(err)); });
    }
}

ModulationDialog::~ModulationDialog()
{
    delete ui;
}

void ModulationDialog::onApplyAM()
{
    if (!m_controller) return;
    m_controller->setAMState(m_channel, ui->chkAMState->isChecked());
    m_controller->setAMFrequency(m_channel, ui->spinAMFreq->value());
    m_controller->setAMDepth(m_channel, ui->spinAMDepth->value());
    appendLog(tr("AM применена: частота=%1 Гц, глубина=%2%")
              .arg(ui->spinAMFreq->value()).arg(ui->spinAMDepth->value()));
}

void ModulationDialog::onApplyFM()
{
    if (!m_controller) return;
    m_controller->setFMState(m_channel, ui->chkFMState->isChecked());
    m_controller->setFMFrequency(m_channel, ui->spinFMFreq->value());
    m_controller->setFMDeviation(m_channel, ui->spinFMDev->value());
    appendLog(tr("FM применена: частота=%1 Гц, девиация=%2 Гц")
              .arg(ui->spinFMFreq->value()).arg(ui->spinFMDev->value()));
}

void ModulationDialog::onQueryAll()
{
    if (!m_controller) return;
    double amFreq = m_controller->queryAMFrequency(m_channel);
    double amDepth = m_controller->queryAMDepth(m_channel);
    bool amOn = m_controller->queryAMState(m_channel);
    double fmFreq = m_controller->queryFMFrequency(m_channel);
    double fmDev = m_controller->queryFMDeviation(m_channel);
    bool fmOn = m_controller->queryFMState(m_channel);

    ui->spinAMFreq->setValue(amFreq);
    ui->spinAMDepth->setValue(amDepth);
    ui->chkAMState->setChecked(amOn);
    ui->spinFMFreq->setValue(fmFreq);
    ui->spinFMDev->setValue(fmDev);
    ui->chkFMState->setChecked(fmOn);

    appendLog(tr("Считано: AM=%1 Гц/%2% [%3], FM=%4 Гц/%5 Гц [%6]")
              .arg(amFreq).arg(amDepth).arg(amOn ? tr("ВКЛ") : tr("ВЫКЛ"))
              .arg(fmFreq).arg(fmDev).arg(fmOn ? tr("ВКЛ") : tr("ВЫКЛ")));
}

void ModulationDialog::appendLog(const QString &msg)
{
    ui->txtLog->appendPlainText(
        QString("[%1] %2").arg(QDateTime::currentDateTime().toString("hh:mm:ss")).arg(msg));
}
