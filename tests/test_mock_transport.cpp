#include <QtTest>
#include "mocktransport.h"

/**
 * @brief Test suite for MockTransport.
 *
 * Smoke test for MockTransport itself. GrattenGa1483Controller integration
 * testing requires refactoring to accept ITransport — that's a separate task.
 */
class TestMockTransport : public QObject
{
    Q_OBJECT

private slots:
    void testOpenClose();
    void testSendCommand();
    void testQueryCommand();
    void testSentCommandsLog();
};

void TestMockTransport::testOpenClose()
{
    MockTransport t;
    QVERIFY(!t.isOpen());
    QVERIFY(t.open());
    QVERIFY(t.isOpen());
    t.close();
    QVERIFY(!t.isOpen());
}

void TestMockTransport::testSendCommand()
{
    MockTransport t;
    t.open();
    QVERIFY(t.sendScpiCommand(":FREQuency:CW 1000000"));
    t.setSendResult(false);
    QVERIFY(!t.sendScpiCommand(":FREQuency:CW 2000000"));
}

void TestMockTransport::testQueryCommand()
{
    MockTransport t;
    t.open();
    t.setResponse("*IDN?", "Atten,GA1483,SN12345,1.0");
    QCOMPARE(t.queryScpiCommand("*IDN?"), QString("Atten,GA1483,SN12345,1.0"));
    QCOMPARE(t.queryScpiCommand("unknown?"), QString());
}

void TestMockTransport::testSentCommandsLog()
{
    MockTransport t;
    t.open();
    t.sendScpiCommand("CMD1");
    t.sendScpiCommand("CMD2");
    t.queryScpiCommand("QUERY?");
    QCOMPARE(t.sentCommands().size(), 3);
    QCOMPARE(t.sentCommands().at(0), QString("CMD1"));
    t.clearSentCommands();
    QVERIFY(t.sentCommands().isEmpty());
}

QTEST_MAIN(TestMockTransport)
#include "test_mock_transport.moc"
