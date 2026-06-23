#include "splashwidget.h"
#include "version.h"
#include <QHBoxLayout>
#include <QSpacerItem>

SplashWidget::SplashWidget(QWidget *parent)
    : QWidget(parent)
    , m_lastDeviceLabel(new QLabel(tr("Последнее устройство: —"), this))
{
    auto *outer = new QVBoxLayout(this);
    outer->setAlignment(Qt::AlignCenter);

    auto *card = new QWidget(this);
    card->setObjectName("splashCard");
    card->setFixedWidth(420);
    auto *vbox = new QVBoxLayout(card);
    vbox->setSpacing(16);
    vbox->setContentsMargins(40, 40, 40, 40);

    // Title
    auto *title = new QLabel(QString(APP_NAME), this);
    title->setObjectName("splashTitle");
    title->setAlignment(Qt::AlignCenter);
    QFont f = title->font();
    f.setPointSize(18);
    f.setBold(true);
    title->setFont(f);

    // Version
    auto *version = new QLabel(QString("v%1").arg(APP_VERSION_STRING), this);
    version->setObjectName("splashVersion");
    version->setAlignment(Qt::AlignCenter);

    // Status icon + text
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

    // Last device info
    m_lastDeviceLabel->setObjectName("splashLastDevice");
    m_lastDeviceLabel->setAlignment(Qt::AlignCenter);

    // Separator
    auto *sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);

    // Hint text
    auto *hint = new QLabel(
        tr("Нажмите <b>Подключить</b> на панели инструментов\n"
           "или выберите устройство через Настройки → Подключение"), this);
    hint->setObjectName("splashHint");
    hint->setAlignment(Qt::AlignCenter);
    hint->setWordWrap(true);

    // Supported devices info
    auto *devInfo = new QLabel(
        tr("Поддерживаемые устройства:\n"
           "• АКИП-3417 (DDS, USB CH375, 2 канала)\n"
           "• Gratten GA1483 (RF, LAN TCP, 1 канал)"), this);
    devInfo->setObjectName("splashDevInfo");
    devInfo->setAlignment(Qt::AlignLeft);

    // Buttons
    auto *btnRow = new QHBoxLayout();
    auto *btnConnect = new QPushButton(tr("Подключить"), this);
    btnConnect->setObjectName("btnConnect");
    auto *btnSettings = new QPushButton(tr("Настройки"), this);

    btnRow->addStretch();
    btnRow->addWidget(btnConnect);
    btnRow->addWidget(btnSettings);
    btnRow->addStretch();

    vbox->addWidget(title);
    vbox->addWidget(version);
    vbox->addLayout(statusRow);
    vbox->addWidget(m_lastDeviceLabel);
    vbox->addWidget(sep);
    vbox->addWidget(hint);
    vbox->addWidget(devInfo);
    vbox->addLayout(btnRow);

    outer->addStretch();
    outer->addWidget(card, 0, Qt::AlignCenter);
    outer->addStretch();

    connect(btnConnect, &QPushButton::clicked, this, &SplashWidget::connectRequested);
    connect(btnSettings, &QPushButton::clicked, this, &SplashWidget::settingsRequested);
}

void SplashWidget::setLastDevice(const QString &name, const QString &address)
{
    if (name.isEmpty()) {
        m_lastDeviceLabel->setText(tr("Последнее устройство: —"));
    } else {
        m_lastDeviceLabel->setText(
            tr("Последнее устройство: %1  (%2)").arg(name).arg(address));
    }
}
