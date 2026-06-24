#pragma once
#include <QDialog>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QElapsedTimer>
#include "iakipcontroller.h"

class DiagnosticsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DiagnosticsDialog(IAkipController *controller, QWidget *parent = nullptr);

private slots:
    void onTestIdn();
    void onTestSetFreq();
    void onTestQueryFreq();
    void onTestSeriesIdn();

private:
    void appendLog(const QString &msg);

    IAkipController *m_controller;
    QElapsedTimer    m_timer;

    QComboBox    *m_cmbChannel;
    QLineEdit    *m_editFreq;
    QLabel       *m_lblIdnTime;
    QLabel       *m_lblSetFreqTime;
    QLabel       *m_lblQueryFreqTime;
    QPlainTextEdit *m_log;
};
