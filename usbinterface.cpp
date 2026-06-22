#include "usbinterface.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>
#include <windows.h>
UsbInterface::UsbInterface(QObject *parent)
    : QObject(parent)
    , m_deviceHandle(INVALID_HANDLE_VALUE)
    , m_isOpen(false)
    , m_writeTimeout(2000)
    , m_readTimeout(2000)
{
}

UsbInterface::~UsbInterface()
{
    close();
}

// Вспомогательный метод для безопасного приведения HANDLE к ULONG
ULONG UsbInterface::handleToULong() const
{
    // В 64-битной системе используем ULONG_PTR для безопасного приведения
    return static_cast<ULONG>(reinterpret_cast<ULONG_PTR>(m_deviceHandle));
}

bool UsbInterface::open(int index)
{
    if (m_isOpen) {
        close();
    }

    // Пробуем открыть устройство
    m_deviceHandle = CH375OpenDevice(index);
    if (m_deviceHandle == INVALID_HANDLE_VALUE) {
        emit errorOccurred("Не удалось открыть устройство. Проверьте подключение и драйвер.");
        return false;
    }

    // Устанавливаем таймауты
    CH375SetTimeout(handleToULong(), m_writeTimeout, m_readTimeout);

    m_isOpen = true;
   // qDebug() << "Устройство АКИП-3417 успешно открыто";
    emit deviceOpened();
    return true;
}

void UsbInterface::close()
{
    if (m_isOpen && m_deviceHandle != INVALID_HANDLE_VALUE) {
        CH375CloseDevice(handleToULong());
        m_deviceHandle = INVALID_HANDLE_VALUE;
        m_isOpen = false;
      //  qDebug() << "Устройство закрыто";
        emit deviceClosed();
    }
}

bool UsbInterface::sendScpiCommand(const QString &command)
{
    if (!m_isOpen) {
        emit errorOccurred("Устройство не открыто");
        return false;
    }

    // Согласно документации: для USB команда заканчивается символом \n (ASCII 10)
    QByteArray data = command.toLatin1() + "\n";

    ULONG length = data.size();
    BOOL result = CH375WriteData(handleToULong(), data.data(), &length);

    if (result && length == (ULONG)data.size()) {
       // qDebug() << "Команда отправлена:" << command;
        return true;
    } else {
        QString error = QString("Ошибка отправки команды. Отправлено %1 из %2 байт")
                            .arg(length).arg(data.size());
        emit errorOccurred(error);
        return false;
    }
}

QString UsbInterface::queryScpiCommand(const QString &command, int timeoutMs)
{
    if (!sendScpiCommand(command)) {
        return QString();
    }

    // Для запросов (с '?') ждем ответ
    if (command.contains('?')) {
        return waitForResponse(timeoutMs);
    }

    return "OK"; // Для команд без запроса возвращаем условное подтверждение
}

bool UsbInterface::setOutput(bool enable, int channel)
{
    QString cmd = QString("OUTP:CH%1 %2").arg(channel == 1 ? "A" : "B").arg(enable ? "ON" : "OFF");
    return sendScpiCommand(cmd);
}

bool UsbInterface::setFrequency(double freqHz, int channel)
{
    // Форматируем частоту. SCPI принимает числа в формате 1.234e+3 или 1234
    QString freqStr = QString::number(freqHz, 'f', 0); // Без дробной части для целых значений
    QString cmd = QString("FREQ:CH%1 %2").arg(channel == 1 ? "A" : "B").arg(freqStr);
    return sendScpiCommand(cmd);
}

QString UsbInterface::getIdentity()
{
    return queryScpiCommand("*IDN?");
}

bool UsbInterface::resetDevice()
{
    return sendScpiCommand("*RST");
}

QString UsbInterface::waitForResponse(int timeoutMs)
{
    QElapsedTimer timer;
    timer.start();

    QByteArray response;
    while (timer.elapsed() < timeoutMs) {
        char buffer[256];
        ULONG length = sizeof(buffer);

        BOOL result = CH375ReadData(handleToULong(), buffer, &length);

        if (result && length > 0) {
            response.append(buffer, length);

            // Проверяем, есть ли в ответе символ конца строки
            if (response.contains('\n') || response.contains('\r')) {
                break;
            }
        }

        // Небольшая пауза, чтобы не грузить процессор
        QThread::msleep(10);
    }

    // Убираем служебные символы из ответа
    response = response.trimmed();
    return QString::fromLatin1(response);
}
bool UsbInterface::isOpen() const
{
    return m_isOpen;
}

bool UsbInterface::setWriteTimeout(int ms)
{
    m_writeTimeout = ms;
    if (m_isOpen) {
        return CH375SetTimeout(handleToULong(), m_writeTimeout, m_readTimeout) != 0;
    }
    return true;
}

bool UsbInterface::setReadTimeout(int ms)
{
    m_readTimeout = ms;
    if (m_isOpen) {
        return CH375SetTimeout(handleToULong(), m_writeTimeout, m_readTimeout) != 0;
    }
    return true;
}

// Заглушки для пока нереализованных методов
bool UsbInterface::setAmplitude(double amplitude, const QString &unit, int channel)
{
    QString cmd = QString("VOLT:CH%1 %2 %3")
    .arg(channel == 1 ? "A" : "B")
        .arg(amplitude)
        .arg(unit.toUpper());
    return sendScpiCommand(cmd);
}

bool UsbInterface::setWaveform(const QString &waveform, int channel)
{
    QString waveUpper = waveform.toUpper();
    QString cmd;

    if (waveUpper == "SIN" || waveUpper == "SINE") {
        cmd = QString("FUNC:CH%1 SIN").arg(channel == 1 ? "A" : "B");
    }
    else if (waveUpper == "SQU" || waveUpper == "SQUARE") {
        cmd = QString("FUNC:CH%1 SQUARE").arg(channel == 1 ? "A" : "B");
    }
    else if (waveUpper == "RAMP") {
        cmd = QString("FUNC:CH%1 RAMP").arg(channel == 1 ? "A" : "B");
    }
    else if (waveUpper == "PULSE") {
        cmd = QString("FUNC:CH%1 PULSE").arg(channel == 1 ? "A" : "B");
    }
    else {
        // Если форма сигнала не распознана, отправляем как есть
        cmd = QString("FUNC:CH%1 %2").arg(channel == 1 ? "A" : "B").arg(waveform);
    }

    return sendScpiCommand(cmd);
}

