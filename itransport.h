#ifndef ITRANSPORT_H
#define ITRANSPORT_H

#include <QObject>
#include <QString>

/**
 * @brief Abstract interface for SCPI transport layer.
 *
 * Defines the contract for any transport implementation (LAN, USB, Mock).
 * Implementers must provide methods to open/close connections and send/receive
 * SCPI commands.
 */
class ITransport : public QObject
{
    Q_OBJECT
public:
    explicit ITransport(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~ITransport() {}

    /// Open a transport connection (e.g., USB device at index, or LAN socket)
    virtual bool open(int index = 0) = 0;

    /// Close the transport connection
    virtual void close() = 0;

    /// Check if transport is open and ready
    virtual bool isOpen() const = 0;

    /// Send a SCPI command (no response expected)
    /// Returns true on success, false on error
    virtual bool sendScpiCommand(const QString &cmd) = 0;

    /// Send a SCPI query and wait for response
    /// Returns the response string (empty if error or timeout)
    virtual QString queryScpiCommand(const QString &cmd) = 0;

signals:
    /// Emitted when an error occurs
    void errorOccurred(const QString &error);
};

#endif // ITRANSPORT_H
