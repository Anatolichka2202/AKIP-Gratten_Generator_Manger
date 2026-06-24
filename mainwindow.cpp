#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsmanager.h"
#include "settingsdialog.h"
#include "languageswitcher.h"
#include "aboutdialog.h"
#include "diagnosticsdialog.h"
#include <QElapsedTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QTextStream>
#include <QTimer>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_controller(nullptr)
    , m_currentType(Unknown)
    , m_channelA(nullptr)
    , m_channelB(nullptr)
    , m_langSwitcher(nullptr)
    , m_sbDevice(nullptr)
    , m_sbConn(nullptr)
    , m_sbLastOp(nullptr)
    , m_splashPage(nullptr)
{
    ui->setupUi(this);
    m_akipPage = ui->stackedWidget->widget(0);
    m_grattenPage = nullptr;

    // Splash screen shown when no device is connected
    m_splashPage = new SplashWidget(this);
    ui->stackedWidget->addWidget(m_splashPage);
    connect(m_splashPage, &SplashWidget::connectRequested,
            this, &MainWindow::on_btnConnect_clicked);
    connect(m_splashPage, &SplashWidget::quickConnectRequested, this,
            [this](const QString &deviceType) {
                switchToDevice(deviceType == "AKIP" ? AKIP : GRATTEN);
            });
    connect(m_splashPage, &SplashWidget::settingsRequested, this, [this]() {
        SettingsDialog dlg(this);
        dlg.exec();
    });

    m_langSwitcher = new LanguageSwitcher(qApp, this);

    setupMenu();

    // Setup status bar
    m_sbDevice = new QLabel(tr("Не подключено"), this);
    m_sbDevice->setMinimumWidth(150);
    statusBar()->addWidget(m_sbDevice);

    m_sbConn = new QLabel(this);
    m_sbConn->setMinimumWidth(150);
    statusBar()->addWidget(m_sbConn, 1);

    m_sbLastOp = new QLabel(tr("—"), this);
    m_sbLastOp->setMinimumWidth(150);
    m_sbLastOp->setAlignment(Qt::AlignRight);
    statusBar()->addPermanentWidget(m_sbLastOp);

    // Create channel widgets for AKIP page
    m_channelA = new ChannelWidget(1, nullptr, this);
    m_channelB = new ChannelWidget(2, nullptr, this);

    // Add to AKIP page layout
    ui->horizontalLayout_2->insertWidget(0, m_channelA);
    ui->horizontalLayout_2->insertWidget(1, m_channelB);

    // Connect channel widgets' log signals to main window
    connect(m_channelA, &ChannelWidget::logMessage, this, &MainWindow::onChannelWidgetLogMessage);
    connect(m_channelB, &ChannelWidget::logMessage, this, &MainWindow::onChannelWidgetLogMessage);

    setChannelControlsEnabled(false);
    updateConnectionStatus();
    logMessage(tr("Программа запущена"));
    showSplash();

    // Setup Help menu with About dialog
    QMenu *helpMenu = menuBar()->addMenu(tr("Справка"));
    QAction *aboutAction = helpMenu->addAction(tr("О программе..."));
    connect(aboutAction, &QAction::triggered, this, [this]() {
        AboutDialog dlg(this);
        dlg.exec();
    });

    // Запускаем автоопределение после короткой задержки
    QTimer::singleShot(100, this, &MainWindow::checkAvailableDevices);
}

MainWindow::~MainWindow()
{
    if (m_controller) {
        m_controller->closeDevice();
        delete m_controller;
    }
    delete ui;
}

// ==================== Меню ====================

void MainWindow::setupMenu()
{
    QMenu *menuSettings = menuBar()->addMenu(tr("Настройки"));
    QAction *actConnection = menuSettings->addAction(tr("Подключение..."));
    connect(actConnection, &QAction::triggered, this, [this]() {
        SettingsDialog dlg(this);
        dlg.exec();
    });

    if (m_langSwitcher) {
        menuSettings->addSeparator();
        menuSettings->addMenu(m_langSwitcher->createLanguageMenu(this));
    }

    QMenu *menuTools = menuBar()->addMenu(tr("Инструменты"));
    QAction *actDiag = menuTools->addAction(tr("Диагностика..."));
    connect(actDiag, &QAction::triggered, this, [this]() {
        DiagnosticsDialog dlg(m_controller, this);
        dlg.exec();
    });
}

// ==================== Логирование и вспомогательные методы ====================

void MainWindow::logMessage(const QString &msg)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    ui->txtLog->append(QString("[%1] %2").arg(timestamp).arg(msg));
}

void MainWindow::setChannelControlsEnabled(bool enabled)
{
    if (m_currentType == GRATTEN) {
        // In Gratten mode, enable the Gratten page instead of channel controls
        if (m_grattenPage) {
            m_grattenPage->setEnabled(enabled);
        }
    } else if (m_currentType == AKIP) {
        // In AKIP mode, enable channel A/B and delays
        if (m_channelA) m_channelA->setEnabled(enabled);
        if (m_channelB) m_channelB->setEnabled(enabled);
        ui->grpDelays->setEnabled(enabled);
    }
}

void MainWindow::updateConnectionStatus()
{
    bool avail = m_controller ? m_controller->isAvailable() : false;
    if (avail) {
        ui->lblStatus->setText(tr("🟢 Подключено"));
        ui->btnConnect->setEnabled(false);
        ui->btnDisconnect->setEnabled(true);
    } else {
        ui->lblStatus->setText(tr("🔴 Отключено"));
        ui->btnConnect->setEnabled(true);
        ui->btnDisconnect->setEnabled(false);
    }
    updateStatusBar();
}

void MainWindow::updateStatusBar()
{
    // Update device type label
    switch (m_currentType) {
    case AKIP:
        m_sbDevice->setText("АКИП-3417");
        break;
    case GRATTEN:
        m_sbDevice->setText("Gratten GA1483");
        break;
    default:
        m_sbDevice->setText(tr("Не подключено"));
        break;
    }

    // Update connection info label
    bool avail = m_controller ? m_controller->isAvailable() : false;
    if (avail && m_currentType == GRATTEN) {
        auto &cfg = SettingsManager::instance();
        m_sbConn->setText(QString("%1:%2")
                         .arg(cfg.grattenHost())
                         .arg(cfg.grattenPort()));
    } else if (avail && m_currentType == AKIP) {
        m_sbConn->setText(tr("USB CH375"));
    } else {
        m_sbConn->setText("");
    }
}

// ==================== Слоты подключения ====================

void MainWindow::on_btnConnect_clicked()
{
    if (m_controller) {
        logMessage(tr("Уже подключено"));
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Выбор устройства"),
                                                              tr("Подключиться к АКИП-3417 (USB)?\n"
                                                                 "(Нажмите No для подключения к Gratten GA1483 по LAN)"),
                                                              QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    if (reply == QMessageBox::Yes) {
        switchToDevice(AKIP);
    } else if (reply == QMessageBox::No) {
        switchToDevice(GRATTEN);
    }
}

void MainWindow::on_btnDisconnect_clicked()
{
    if (m_controller) {
        m_controller->closeDevice();
        delete m_controller;
        m_controller = nullptr;
        m_currentType = Unknown;
        ui->lblIdn->setText(tr("—"));
        ui->lblDeviceType->setText(tr("—"));
        setChannelControlsEnabled(false);
        updateConnectionStatus();
        updateStatusBar();
        logMessage(tr("Устройство отключено"));
        showSplash();
    }
}

void MainWindow::on_btnIdn_clicked()
{
    if (!m_controller || !m_controller->isAvailable()) return;
    QElapsedTimer t;
    t.start();
    QString idn = m_controller->getIdentity();
    ui->lblIdn->setText(idn);
    logMessage(QString(tr("*IDN? → %1 мс  |  %2")).arg(t.elapsed()).arg(idn));
}

// ==================== Лог ====================

void MainWindow::on_btnLogClear_clicked()
{
    ui->txtLog->clear();
}

void MainWindow::on_btnLogSave_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить лог"), "akip_log.txt", tr("Text files (*.txt)"));
    if (fileName.isEmpty()) return;
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << ui->txtLog->toPlainText();
        file.close();
        logMessage(QString(tr("Лог сохранён в %1")).arg(fileName));
    } else {
        logMessage(tr("Ошибка сохранения лога"));
    }
}

// ==================== Слоты от контроллера ====================

void MainWindow::onControllerAvailabilityChanged(bool available)
{
    updateConnectionStatus();
    logMessage(QString(tr("Доступность устройства изменилась: %1")).arg(available ? tr("доступно") : tr("недоступно")));
}

void MainWindow::onControllerError(const QString &error)
{
    logMessage(tr("ОШИБКА: ") + error);
}

void MainWindow::onChannelWidgetLogMessage(const QString &msg)
{
    logMessage(msg);
}

// ==================== Автоопределение и выбор устройства ====================

void MainWindow::checkAvailableDevices()
{
    logMessage(tr("Поиск доступных устройств..."));
    bool akipOk = false;
    QString akipIdn;
    bool grattenOk = false;
    QString grattenIdn;

    // --- Проверка АКИП (USB) ---
    {
        AkipFacade testAkip;
        if (testAkip.openDevice(0)) {
            akipIdn = testAkip.getIdentity();
            akipOk = !akipIdn.isEmpty();
            testAkip.closeDevice();
        }
    }

    // --- Проверка Граттен (LAN, IP из настроек) ---
    {
        auto &cfg = SettingsManager::instance();
        GrattenGa1483Controller testGratten;
        testGratten.setConnectionParameters(cfg.grattenHost(), cfg.grattenPort());
        if (testGratten.openDevice()) {
            grattenIdn = testGratten.getIdentity();
            grattenOk = !grattenIdn.isEmpty();
            testGratten.closeDevice();
        }
    }

    // Анализ результатов
    if (akipOk && grattenOk) {
        logMessage(tr("Найдены оба устройства. Требуется выбор."));
        showDeviceSelectionDialog(akipIdn, grattenIdn);
    }
    else if (akipOk) {
        logMessage(tr("Найден только АКИП. Автоподключение."));
        switchToDevice(AKIP);
    }
    else if (grattenOk) {
        logMessage(tr("Найден только Gratten. Автоподключение."));
        switchToDevice(GRATTEN);
    }
    else {
        logMessage(tr("Ни одно устройство не найдено."));
        showSplash();
    }
}

void MainWindow::showDeviceSelectionDialog(const QString &akipIdn, const QString &grattenIdn)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("Выбор устройства"));
    msgBox.setText(tr("Обнаружено несколько устройств. Выберите, к какому подключиться:"));
    msgBox.setIcon(QMessageBox::Question);

    QPushButton *btnAkip = msgBox.addButton(tr("АКИП-3417 (USB)"), QMessageBox::AcceptRole);
    QPushButton *btnGratten = msgBox.addButton(tr("Gratten GA1483 (LAN)"), QMessageBox::AcceptRole);
    msgBox.addButton(QMessageBox::Cancel);

    msgBox.exec();

    if (msgBox.clickedButton() == btnAkip) {
        switchToDevice(AKIP);
    } else if (msgBox.clickedButton() == btnGratten) {
        switchToDevice(GRATTEN);
    } else {
        logMessage(tr("Выбор отменён."));
    }
}

void MainWindow::switchToDevice(DeviceType type)
{
    // Удаляем старый контроллер
    if (m_controller) {
        m_controller->closeDevice();
        delete m_controller;
        m_controller = nullptr;
    }

    // Создаём новый
    switch (type) {
    case AKIP:
        m_controller = new AkipFacade(this);
        break;
    case GRATTEN: {
        auto &cfg = SettingsManager::instance();
        m_controller = new GrattenGa1483Controller(this);
        static_cast<GrattenGa1483Controller*>(m_controller)
            ->setConnectionParameters(cfg.grattenHost(), cfg.grattenPort());
        break;
    }
    default:
        return;
    }

    // Подключаем сигналы
    connect(m_controller, &IAkipController::availabilityChanged,
            this, &MainWindow::onControllerAvailabilityChanged);
    connect(m_controller, &IAkipController::errorOccurred,
            this, &MainWindow::onControllerError);

    // Пытаемся открыть
    if (m_controller->openDevice()) {
        m_currentType = type;
        QString idn = m_controller->getIdentity();
        ui->lblIdn->setText(idn);
        logMessage(QString(tr("Подключено устройство: %1")).arg(idn));

        setupForDeviceType(type);

        SettingsManager::instance().setLastDeviceType(type == AKIP ? "AKIP" : "GRATTEN");
        ui->lblDeviceType->setText(type == AKIP ? tr("АКИП-3417") : tr("Gratten GA1483"));
    } else {
        logMessage(tr("Ошибка открытия выбранного устройства"));
        delete m_controller;
        m_controller = nullptr;
        m_currentType = Unknown;
        updateConnectionStatus();
        updateStatusBar();
        showSplash();
        return;
    }
    updateConnectionStatus();
    updateStatusBar();
}

void MainWindow::setupForDeviceType(DeviceType type)
{
    // Удаляем предыдущую страницу Gratten, если она была
    if (m_grattenPage) {
        ui->stackedWidget->removeWidget(m_grattenPage);
        delete m_grattenPage;
        m_grattenPage = nullptr;
    }

    if (type == AKIP) {
        ui->stackedWidget->setCurrentWidget(m_akipPage);
        m_channelA->setController(m_controller);
        m_channelB->setController(m_controller);
        setChannelControlsEnabled(true);
    } else if (type == GRATTEN) {
        m_grattenPage = new GrattenControlWidget(m_controller, this);
        int idx = ui->stackedWidget->addWidget(m_grattenPage);
        ui->stackedWidget->setCurrentIndex(idx);

        setChannelControlsEnabled(true);
    } else { // Unknown
        setChannelControlsEnabled(false);
    }
}

void MainWindow::showSplash()
{
    auto &cfg = SettingsManager::instance();
    QString lastType = cfg.lastDeviceType();
    if (lastType == "AKIP") {
        m_splashPage->setLastDevice(tr("АКИП-3417"), tr("USB"), "AKIP");
    } else if (lastType == "GRATTEN") {
        m_splashPage->setLastDevice(tr("Gratten GA1483"),
            QString("%1:%2").arg(cfg.grattenHost()).arg(cfg.grattenPort()),
            "GRATTEN");
    } else {
        m_splashPage->setLastDevice("", "", "");
    }
    ui->stackedWidget->setCurrentWidget(m_splashPage);
}
