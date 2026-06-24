#include "diagnosticsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QDateTime>
#include <QDialogButtonBox>

DiagnosticsDialog::DiagnosticsDialog(IAkipController *controller, QWidget *parent)
    : QDialog(parent)
    , m_controller(controller)
    , m_cmbChannel(new QComboBox(this))
    , m_editFreq(new QLineEdit("1000", this))
    , m_lblIdnTime(new QLabel(tr("—"), this))
    , m_lblSetFreqTime(new QLabel(tr("—"), this))
    , m_lblQueryFreqTime(new QLabel(tr("—"), this))
    , m_log(new QPlainTextEdit(this))
{
    setWindowTitle(tr("Диагностика подключения"));
    setMinimumWidth(520);

    m_cmbChannel->addItem("1 / A");
    m_cmbChannel->addItem("2 / B");

    m_log->setReadOnly(true);
    m_log->setFont(QFont("Courier New", 9));
    m_log->setMinimumHeight(180);

    // ── Timing tests group ─────────────────────────────────────────────────
    auto *grpTiming = new QGroupBox(tr("Измерение задержек"), this);
    auto *grid = new QGridLayout(grpTiming);

    grid->addWidget(new QLabel(tr("Канал:"), this),              0, 0);
    grid->addWidget(m_cmbChannel,                                 0, 1);
    grid->addWidget(new QLabel(tr("Тест. частота (Гц):"), this), 0, 2);
    grid->addWidget(m_editFreq,                                   0, 3);

    auto mkRow = [&](int row, QPushButton *btn, QLabel *lbl) {
        grid->addWidget(btn,                                row, 0, 1, 2);
        grid->addWidget(lbl,                                row, 2);
        grid->addWidget(new QLabel(tr("мс"), this),         row, 3);
    };

    auto *btnIdn      = new QPushButton(tr("*IDN?"),                    this);
    auto *btnSetFreq  = new QPushButton(tr("Set Freq → время записи"),  this);
    auto *btnQryFreq  = new QPushButton(tr("Query Freq → время ответа"),this);
    auto *btnSeries   = new QPushButton(tr("Серия 10× *IDN? (статистика)"), this);

    mkRow(1, btnIdn,     m_lblIdnTime);
    mkRow(2, btnSetFreq, m_lblSetFreqTime);
    mkRow(3, btnQryFreq, m_lblQueryFreqTime);
    grid->addWidget(btnSeries, 4, 0, 1, 4);

    // ── Log ────────────────────────────────────────────────────────────────
    auto *grpLog = new QGroupBox(tr("Лог"), this);
    auto *logLayout = new QVBoxLayout(grpLog);
    auto *btnClear = new QPushButton(tr("Очистить"), this);
    btnClear->setMaximumWidth(100);
    logLayout->addWidget(btnClear, 0, Qt::AlignLeft);
    logLayout->addWidget(m_log);

    auto *bb = new QDialogButtonBox(QDialogButtonBox::Close, this);

    auto *vbox = new QVBoxLayout(this);
    vbox->addWidget(grpTiming);
    vbox->addWidget(grpLog);
    vbox->addWidget(bb);

    connect(btnIdn,    &QPushButton::clicked, this, &DiagnosticsDialog::onTestIdn);
    connect(btnSetFreq,&QPushButton::clicked, this, &DiagnosticsDialog::onTestSetFreq);
    connect(btnQryFreq,&QPushButton::clicked, this, &DiagnosticsDialog::onTestQueryFreq);
    connect(btnSeries, &QPushButton::clicked, this, &DiagnosticsDialog::onTestSeriesIdn);
    connect(btnClear,  &QPushButton::clicked, m_log, &QPlainTextEdit::clear);
    connect(bb, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void DiagnosticsDialog::appendLog(const QString &msg)
{
    m_log->appendPlainText(
        QString("[%1] %2").arg(QDateTime::currentDateTime().toString("hh:mm:ss.zzz"), msg));
}

void DiagnosticsDialog::onTestIdn()
{
    if (!m_controller || !m_controller->isAvailable()) {
        appendLog(tr("Устройство недоступно"));
        return;
    }
    m_timer.start();
    QString idn = m_controller->getIdentity();
    qint64 ms = m_timer.elapsed();
    m_lblIdnTime->setText(QString::number(ms));
    appendLog(tr("*IDN? → %1 мс  |  %2").arg(ms).arg(idn));
}

void DiagnosticsDialog::onTestSetFreq()
{
    if (!m_controller || !m_controller->isAvailable()) {
        appendLog(tr("Устройство недоступно"));
        return;
    }
    bool ok;
    double freq = m_editFreq->text().toDouble(&ok);
    if (!ok || freq <= 0) { appendLog(tr("Некорректная частота")); return; }

    int ch = m_cmbChannel->currentIndex() + 1;
    m_timer.start();
    bool res = m_controller->setFrequency(ch, freq);
    qint64 ms = m_timer.elapsed();
    m_lblSetFreqTime->setText(QString::number(ms));
    appendLog(tr("Set Freq ch%1 %2 Гц → %3 мс  [%4]")
                  .arg(ch).arg(freq).arg(ms).arg(res ? tr("OK") : tr("FAIL")));
}

void DiagnosticsDialog::onTestQueryFreq()
{
    if (!m_controller || !m_controller->isAvailable()) {
        appendLog(tr("Устройство недоступно"));
        return;
    }
    int ch = m_cmbChannel->currentIndex() + 1;
    m_timer.start();
    double freq = m_controller->queryFrequency(ch);
    qint64 ms = m_timer.elapsed();
    m_lblQueryFreqTime->setText(QString::number(ms));
    appendLog(tr("Query Freq ch%1 → %2 Гц  |  %3 мс").arg(ch).arg(freq).arg(ms));
}

void DiagnosticsDialog::onTestSeriesIdn()
{
    if (!m_controller || !m_controller->isAvailable()) {
        appendLog(tr("Устройство недоступно"));
        return;
    }
    const int N = 10;
    QList<qint64> times;
    appendLog(tr("Серия %1× *IDN? ...").arg(N));
    for (int i = 0; i < N; ++i) {
        m_timer.start();
        QString idn = m_controller->getIdentity();
        qint64 ms = m_timer.elapsed();
        if (!idn.isEmpty()) times.append(ms);
        else appendLog(tr("  #%1 — нет ответа").arg(i + 1));
    }
    if (times.isEmpty()) { appendLog(tr("Нет успешных ответов")); return; }

    qint64 sum = 0, mn = times[0], mx = times[0];
    for (qint64 t : times) { sum += t; mn = qMin(mn, t); mx = qMax(mx, t); }
    appendLog(tr("Результат (%1/%2): мин=%3 мс  макс=%4 мс  сред=%5 мс")
                  .arg(times.size()).arg(N).arg(mn).arg(mx)
                  .arg(double(sum) / times.size(), 0, 'f', 1));
}
