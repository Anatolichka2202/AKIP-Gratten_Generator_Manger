#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "settingsmanager.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    loadSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::loadSettings()
{
    auto &s = SettingsManager::instance();
    ui->editHost->setText(s.grattenHost());
    ui->spinPort->setValue(s.grattenPort());
    ui->spinConnTimeout->setValue(s.grattenConnectTimeoutMs());
    ui->spinRespTimeout->setValue(s.grattenResponseTimeoutMs());
}

void SettingsDialog::saveSettings()
{
    auto &s = SettingsManager::instance();
    s.setGrattenHost(ui->editHost->text().trimmed());
    s.setGrattenPort(static_cast<quint16>(ui->spinPort->value()));
    s.setGrattenConnectTimeoutMs(ui->spinConnTimeout->value());
    s.setGrattenResponseTimeoutMs(ui->spinRespTimeout->value());
}

void SettingsDialog::accept()
{
    saveSettings();
    QDialog::accept();
}
