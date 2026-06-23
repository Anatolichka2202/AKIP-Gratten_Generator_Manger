#ifndef SWEEPDIALOG_H
#define SWEEPDIALOG_H

#include <QDialog>
#include "iakipcontroller.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SweepDialog; }
QT_END_NAMESPACE

class SweepDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SweepDialog(IAkipController *controller, int channel, QWidget *parent = nullptr);
    ~SweepDialog();

private slots:
    void onApply();
    void onStop();
    void onQuery();

private:
    Ui::SweepDialog *ui;
    IAkipController *m_controller;
    int m_channel;
    void appendLog(const QString &msg);
};

#endif // SWEEPDIALOG_H
