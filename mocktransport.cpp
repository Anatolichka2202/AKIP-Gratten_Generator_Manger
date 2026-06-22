#include "mocktransport.h"

MockTransport::MockTransport(QObject *parent)
    : ITransport(parent)
    , m_open(false)
    , m_sendResult(true)
{
}

bool MockTransport::open(int index)
{
    Q_UNUSED(index);
    m_open = true;
    return true;
}

void MockTransport::close()
{
    m_open = false;
}

bool MockTransport::isOpen() const
{
    return m_open;
}

bool MockTransport::sendScpiCommand(const QString &cmd)
{
    m_sentCommands.append(cmd);
    return m_sendResult;
}

QString MockTransport::queryScpiCommand(const QString &cmd)
{
    m_sentCommands.append(cmd);
    return m_responses.value(cmd, QString());
}

void MockTransport::setResponse(const QString &query, const QString &response)
{
    m_responses[query] = response;
}

void MockTransport::setSendResult(bool result)
{
    m_sendResult = result;
}

QStringList MockTransport::sentCommands() const
{
    return m_sentCommands;
}

void MockTransport::clearSentCommands()
{
    m_sentCommands.clear();
}
