#ifndef MOCKTRANSPORT_H
#define MOCKTRANSPORT_H

#include "itransport.h"
#include <QMap>
#include <QStringList>

/**
 * @brief Mock SCPI transport for unit testing.
 *
 * Simulates a SCPI device without requiring actual hardware.
 * Allows tests to set up predefined responses and verify sent commands.
 */
class MockTransport : public ITransport
{
    Q_OBJECT
public:
    explicit MockTransport(QObject *parent = nullptr);

    // ITransport interface
    bool open(int index = 0) override;
    void close() override;
    bool isOpen() const override;
    bool sendScpiCommand(const QString &cmd) override;
    QString queryScpiCommand(const QString &cmd) override;

    // Test helpers
    /// Register a response for a query command
    void setResponse(const QString &query, const QString &response);

    /// Set the result that sendScpiCommand() will return
    void setSendResult(bool result);

    /// Get list of all sent/queried commands in order
    QStringList sentCommands() const;

    /// Clear the sent commands log
    void clearSentCommands();

private:
    bool m_open;
    bool m_sendResult;
    QMap<QString, QString> m_responses;  // query cmd → response
    QStringList m_sentCommands;          // log of all sent commands
};

#endif // MOCKTRANSPORT_H
