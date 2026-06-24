#include "splashwidget.h"
#include "version.h"
#include <QHBoxLayout>

SplashWidget::SplashWidget(QWidget *parent)
    : QWidget(parent)
    , m_lastDeviceLabel(new QLabel(tr("Последнее устройство: —"), this))
    , m_btnQuickConnect(new QPushButton(this))
    , m_btnOther(new QPushButton(tr("Выбрать устройство..."), this))
{
    auto *outer = new QVBoxLayout(this);
    outer->setAlignment(Qt::AlignCenter);

    auto *card = new QWidget(this);
    card->setObjectName("splashCard");
    card->setFixedWidth(440);
    auto *vbox = new QVBoxLayout(card);
    vbox->setSpacing(14);
    vbox->setContentsMargins(40, 40, 40, 40);

    // ── Title ──────────────────────────────────────────────────────────────
    auto *title = new QLabel(QString(APP_NAME), this);
    title->setObjectName("splashTitle");
    title->setAlignment(Qt::AlignCenter);
    QFont f = title->font();
    f.setPointSize(18);
    f.setBold(true);
    title->setFont(f);

    auto *version = new QLabel(QString("v%1").arg(APP_VERSION_STRING), this);
    version->setObjectName("splashVersion");
    version->setAlignment(Qt::AlignCenter);

    // ── Status row ─────────────────────────────────────────────────────────
    auto *statusRow = new QHBoxLayout();
    auto *statusDot = new QLabel("⬤", this);
    statusDot->setObjectName("splashStatusDot");
    statusDot->setStyleSheet("color: #f38ba8; font-size: 10px;");
    auto *statusText = new QLabel(tr("Устройство не подключено"), this);
    statusText->setObjectName("splashStatusText");
    statusRow->addStretch();
    statusRow->addWidget(statusDot);
    statusRow->addSpacing(6);
    statusRow->addWidget(statusText);
    statusRow->addStretch();

    // ── Last device label ──────────────────────────────────────────────────
    m_lastDeviceLabel->setObjectName("splashLastDevice");
    m_lastDeviceLabel->setAlignment(Qt::AlignCenter);

    auto *sep1 = new QFrame(this);
    sep1->setFrameShape(QFrame::HLine);

    // ── Quick-reconnect button (hidden until setLastDevice is called) ───────
    m_btnQuickConnect->setObjectName("btnQuickConnect");
    m_btnQuickConnect->setVisible(false);

    // ── Hint ───────────────────────────────────────────────────────────────
    auto *hint = new QLabel(
        tr("Подключитесь к устройству через кнопку ниже\n"
           "или настройте параметры через кнопку Настройки."), this);
    hint->setObjectName("splashHint");
    hint->setAlignment(Qt::AlignCenter);
    hint->setWordWrap(true);

    auto *sep2 = new QFrame(this);
    sep2->setFrameShape(QFrame::HLine);

    // ── Supported devices ──────────────────────────────────────────────────
    auto *devInfo = new QLabel(
        tr("Поддерживаемые устройства:\n"
           "• АКИП-3417 (DDS, USB CH375, 2 канала)\n"
           "• Gratten GA1483 (RF, LAN TCP, 1 канал)"), this);
    devInfo->setObjectName("splashDevInfo");
    devInfo->setAlignment(Qt::AlignLeft);

    // ── Bottom button row ──────────────────────────────────────────────────
    auto *btnRow = new QHBoxLayout();
    auto *btnSettings = new QPushButton(tr("Настройки"), this);
    btnRow->addStretch();
    btnRow->addWidget(m_btnOther);
    btnRow->addWidget(btnSettings);
    btnRow->addStretch();

    vbox->addWidget(title);
    vbox->addWidget(version);
    vbox->addLayout(statusRow);
    vbox->addWidget(m_lastDeviceLabel);
    vbox->addWidget(sep1);
    vbox->addWidget(m_btnQuickConnect);
    vbox->addWidget(hint);
    vbox->addWidget(sep2);
    vbox->addWidget(devInfo);
    vbox->addLayout(btnRow);

    outer->addStretch();
    outer->addWidget(card, 0, Qt::AlignCenter);
    outer->addStretch();

    connect(m_btnOther,    &QPushButton::clicked, this, &SplashWidget::connectRequested);
    connect(btnSettings,   &QPushButton::clicked, this, &SplashWidget::settingsRequested);
}

void SplashWidget::setLastDevice(const QString &name, const QString &address,
                                  const QString &deviceType)
{
    if (name.isEmpty()) {
        m_lastDeviceLabel->setText(tr("Последнее устройство: —"));
        m_btnQuickConnect->setVisible(false);
        return;
    }

    m_lastDeviceLabel->setText(
        tr("Последнее устройство: %1  (%2)").arg(name, address));

    m_btnQuickConnect->setText(tr("⟳ Переподключиться к %1").arg(name));
    m_btnQuickConnect->setVisible(true);

    // Reconnect the signal with the current deviceType captured
    disconnect(m_btnQuickConnect, nullptr, nullptr, nullptr);
    connect(m_btnQuickConnect, &QPushButton::clicked, this, [this, deviceType]() {
        emit quickConnectRequested(deviceType);
    });
}
