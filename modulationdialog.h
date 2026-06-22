#ifndef MODULATIONDIALOG_H
#define MODULATIONDIALOG_H

#include <QDialog>
#include "iakipcontroller.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ModulationDialog; }
QT_END_NAMESPACE

class ModulationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ModulationDialog(IAkipController *controller, int channel, QWidget *parent = nullptr);
    ~ModulationDialog();

private slots:
    void onApplyAM();
    void onApplyFM();
    void onQueryAll();

private:
    Ui::ModulationDialog *ui;
    IAkipController *m_controller;
    int m_channel;
    void appendLog(const QString &msg);
};

#endif // MODULATIONDIALOG_H
