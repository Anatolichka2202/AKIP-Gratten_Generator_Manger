#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsmanager.h"
#include "settingsdialog.h"
#include "languageswitcher.h"
#include "aboutdialog.h"
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
{
    ui->setupUi(this);
    m_akipPage = ui->stackedWidget->widget(0);
    m_grattenPage = nullptr;

    m_langSwitcher = new LanguageSwitcher(qApp, this);

    setupMenu();

    // Create channel widgets for AKIP page
    m_channelA = new ChannelWidget(1, nullptr, this);
    m_channelB = new ChannelWidget(2, nullptr, this);

    // Add to AKIP page layout
    ui->horizontalLayout_2->insertWidget(0, m_channelA);
    ui->horizontalLayout_2->insertWidget(1, m_channelB);

    // Remove the old group boxes from UI
    ui->grpChannelA->hide();
    ui->grpChannelB->hide();

    // Connect channel widgets' log signals to main window
    connect(m_channelA, &ChannelWidget::logMessage, this, &MainWindow::onChannelWidgetLogMessage);
    connect(m_channelB, &ChannelWidget::logMessage, this, &MainWindow::onChannelWidgetLogMessage);

    setChannelControlsEnabled(false);
    updateConnectionStatus();
    logMessage(tr("Программа запущена"));

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
}

// ==================== Логирование и вспомогательные методы ====================

void MainWindow::logMessage(const QString &msg)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    ui->txtLog->append(QString("[%1] %2").arg(timestamp).arg(msg));
}

void MainWindow::setChannelControlsEnabled(bool enabled)
{
    m_channelA->setEnabled(enabled);
    m_channelB->setEnabled(enabled);
    ui->grpDelays->setEnabled(enabled);
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
        ui->lblIdn->setText("—");
        ui->lblDeviceType->setText("—");
        setChannelControlsEnabled(false);
        updateConnectionStatus();
        logMessage(tr("Устройство отключено"));
    }
}

void MainWindow::on_btnIdn_clicked()
{
    if (!m_controller || !m_controller->isAvailable()) return;
    m_timer.start();
    QString idn = m_controller->getIdentity();
    qint64 elapsed = m_timer.elapsed();
    ui->lblIdn->setText(idn);
    logMessage(QString(tr("*IDN? запрошен, время ответа: %1 мс, ответ: %2")).arg(elapsed).arg(idn));
}

// ==================== Тестирование задержек ====================

void MainWindow::on_btnTestIdn_clicked()
{
    if (!m_controller || !m_controller->isAvailable()) return;
    m_timer.start();
    QString idn = m_controller->getIdentity();
    qint64 elapsed = m_timer.elapsed();
    ui->lblTestIdnTime->setText(QString::number(elapsed));
    logMessage(QString(tr("[ТЕСТ] *IDN? время ответа: %1 мс, ответ: %2")).arg(elapsed).arg(idn));
}

void MainWindow::on_btnTestSetFreq_clicked()
{
    if (!m_controller || !m_controller->isAvailable()) return;
    int channel = ui->cmbTestChannel->currentIndex() + 1;
    double freq = ui->editTestFreq->text().toDouble();
    if (freq <= 0) {
        logMessage(tr("[ТЕСТ] Некорректная частота"));
        return;
    }
    m_timer.start();
    bool ok = m_controller->setFrequency(channel, freq);
    qint64 elapsed = m_timer.elapsed();
    if (ok) {
        ui->lblTestSetFreqTime->setText(QString::number(elapsed));
        logMessage(QString(tr("[ТЕСТ] Установка частоты канал %1 за %2 мс")).arg(channel).arg(elapsed));
    } else {
        logMessage(tr("[ТЕСТ] Ошибка отправки команды"));
    }
}

void MainWindow::on_btnTestQueryFreq_clicked()
{
    if (!m_controller || !m_controller->isAvailable()) return;
    int channel = ui->cmbTestChannel->currentIndex() + 1;
    m_timer.start();
    double freq = m_controller->queryFrequency(channel);
    qint64 elapsed = m_timer.elapsed();
    if (freq > 0) {
        ui->lblTestQueryFreqTime->setText(QString::number(elapsed));
        logMessage(QString(tr("[ТЕСТ] Запрос частоты канал %1: время ответа %2 мс, значение %3 Гц"))
                       .arg(channel).arg(elapsed).arg(freq));
    } else {
        logMessage(tr("[ТЕСТ] Ошибка запроса"));
    }
}

void MainWindow::on_btnTestSeriesIdn_clicked()
{
    if (!m_controller || !m_controller->isAvailable()) return;
    const int count = 10;
    QList<qint64> times;
    logMessage(QString(tr("[ТЕСТ] Серия %1 запросов *IDN? ...")).arg(count));
    for (int i = 0; i < count; ++i) {
        m_timer.start();
        QString idn = m_controller->getIdentity();
        qint64 elapsed = m_timer.elapsed();
        if (!idn.isEmpty()) {
            times.append(elapsed);
        } else {
            logMessage(QString(tr("[ТЕСТ] Ошибка в запросе #%1")).arg(i+1));
        }
    }
    if (times.isEmpty()) {
        logMessage(tr("[ТЕСТ] Нет успешных запросов"));
        return;
    }
    qint64 sum = 0, min = times[0], max = times[0];
    for (qint64 t : times) {
        sum += t;
        if (t < min) min = t;
        if (t > max) max = t;
    }
    double avg = double(sum) / times.size();
    logMessage(QString(tr("[ТЕСТ] Статистика *IDN? (%1 запросов): мин=%2 мс, макс=%3 мс, сред=%4 мс"))
                   .arg(times.size()).arg(min).arg(max).arg(avg, 0, 'f', 2));
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
        logMessage(tr("Ни одно устройство не найдено. Подключитесь вручную."));
        QMessageBox::information(this, tr("Поиск устройств"),
                                 tr("Не удалось обнаружить ни АКИП (USB), ни Gratten (LAN).\n"
                                    "Проверьте подключение и нажмите кнопку Подключиться для выбора."));
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

        ui->lblDeviceType->setText(type == AKIP ? "АКИП-3417" : "Gratten GA1483");
    } else {
        logMessage(tr("Ошибка открытия выбранного устройства"));
        delete m_controller;
        m_controller = nullptr;
        m_currentType = Unknown;
    }
    updateConnectionStatus();
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
