#ifndef USBINTERFACE_H
#define USBINTERFACE_H

#include <QObject>
#include <QByteArray>
#include <QString>

#ifndef NO_USB_INTERFACE

// Отключаем макросы min/max из Windows.h (если они там есть)
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>

// Временно определяем min и max как функциональные макросы,
// чтобы CH375DLL_EN.H не определял свои версии.
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#include "ch375_sdk/CH375DLL_EN.H"

// Удаляем временные макросы, чтобы они не мешали стандартным функциям.
#undef min
#undef max

#endif // NO_USB_INTERFACE

class UsbInterface : public QObject
{
    Q_OBJECT

public:
    explicit UsbInterface(QObject *parent = nullptr);
    ~UsbInterface();

    // Основные методы
    bool open(int index = 0);
    void close();
    bool isOpen() const;

    // Работа с командами
    bool sendScpiCommand(const QString &command);
    QString queryScpiCommand(const QString &command, int timeoutMs = 1000);

    // Утилитарные методы
    bool setWriteTimeout(int ms);
    bool setReadTimeout(int ms);



    // SCPI команды (базовый набор)
    bool setOutput(bool enable, int channel = 1);
    bool setFrequency(double freqHz, int channel = 1);
    bool setAmplitude(double amplitude, const QString &unit = "VPP", int channel = 1);
    bool setWaveform(const QString &waveform, int channel = 1);
    QString getIdentity();
    bool resetDevice();

signals:
    void deviceOpened();
    void deviceClosed();
    void errorOccurred(const QString &error);

private:
#ifndef NO_USB_INTERFACE
    HANDLE m_deviceHandle;
    int m_writeTimeout;
    int m_readTimeout;
    bool m_isOpen;

    // Вспомогательный метод для безопасного приведения HANDLE к ULONG
    ULONG handleToULong() const;

    bool sendRawData(const QByteArray &data);
    QByteArray readRawData(int maxSize = 1024, int timeoutMs = 1000);
    QString waitForResponse(int timeoutMs);
#else
    bool m_isOpen = false;
#endif
};

#endif // USBINTERFACE_H
