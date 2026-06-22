#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "version.h"
#include <QSysInfo>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setFixedSize(400, 280);

    ui->lblAppName->setText(APP_NAME);
    ui->lblVersion->setText(tr("Версия: %1").arg(APP_VERSION_STRING));
    ui->lblOrg->setText(tr("Организация: %1").arg(APP_ORG));
    ui->lblDesc->setText(tr("Инструмент управления генераторами сигналов АКИП-3417 и Gratten GA1483.\nПоддерживает AM, FM, PM, Sweep, PULM, LF через SCPI."));
    ui->lblQt->setText(tr("Qt версия: %1 | Платформа: %2").arg(QT_VERSION_STR).arg(QSysInfo::prettyProductName()));

    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
