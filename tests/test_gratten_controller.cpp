#include <QtTest>
#include <QObject>
#include "grattenga1483controller.h"
#include "mocktransport.h"

/**
 * @brief Test suite for GrattenGa1483Controller using MockTransport.
 *
 * These tests verify that GrattenGa1483Controller sends the correct SCPI
 * commands for various high-level operations.
 *
 * NOTE: Since GrattenGa1483Controller directly owns a LanInterface member,
 * we test by creating a subclass that exposes the transport layer for testing.
 */

class TestableGrattenGa1483Controller : public GrattenGa1483Controller
{
public:
    explicit TestableGrattenGa1483Controller(MockTransport *mockTransport, QObject *parent = nullptr)
        : GrattenGa1483Controller(parent)
        , m_mockTransport(mockTransport)
    {
    }

    MockTransport *getMockTransport() const { return m_mockTransport; }

    // Override sendCommand and queryCommand to use mock transport
    bool sendCommand(const QString &cmd) override
    {
        if (!m_mockTransport) return false;
        return m_mockTransport->sendScpiCommand(cmd);
    }

    QString queryCommand(const QString &cmd) override
    {
        if (!m_mockTransport) return QString();
        return m_mockTransport->queryScpiCommand(cmd);
    }

    bool isTransportAvailable() const override
    {
        if (!m_mockTransport) return false;
        return m_mockTransport->isOpen();
    }

    QString querySystemError() override
    {
        // Simulate no system error
        return "0";
    }

    bool openDevice(int index = 0) override
    {
        Q_UNUSED(index);
        if (!m_mockTransport || !m_mockTransport->isOpen()) {
            if (m_mockTransport)
                m_mockTransport->open();
            else
                return false;
        }
        m_mockTransport->setResponse(":FREQuency:CW?", "1000000");
        m_mockTransport->setResponse(":OUTPut:STATE?", "1");
        m_mockTransport->setResponse(":POWer:LEVEL?", "10");
        m_mockTransport->setResponse(":AM:INTernal:FREQuency?", "1000");
        m_mockTransport->setResponse(":AM:DEPTh?", "50");
        m_mockTransport->setResponse(":AM:STATE?", "1");
        m_mockTransport->setResponse(":SYSTem:ERRor?", "0");
        return true;
    }

private:
    MockTransport *m_mockTransport;
};

class TestGrattenController : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test setFrequency
    void testSetFrequency_Valid();
    void testSetFrequency_OutOfRange();

    // Test setOutput
    void testSetOutput_Enable();
    void testSetOutput_Disable();

    // Test setAMState
    void testSetAMState_Enable();
    void testSetAMState_Disable();

    // Test setSweepStart
    void testSetSweepStart_Valid();

    // Test setAMFrequency
    void testSetAMFrequency_Valid();

    // Test setAMDepth
    void testSetAMDepth_Valid();

    // Test query operations
    void testQueryFrequency_Valid();
    void testGetIdentity();

    // Test power
    void testSetPower_Valid();
    void testSetPower_OutOfRange();

private:
    MockTransport *m_mockTransport;
    TestableGrattenGa1483Controller *m_controller;
};

void TestGrattenController::initTestCase()
{
    // Called once before all tests
}

void TestGrattenController::cleanupTestCase()
{
    // Called once after all tests
}

void TestGrattenController::init()
{
    // Called before each test
    m_mockTransport = new MockTransport(nullptr);
    m_mockTransport->open();

    m_controller = new TestableGrattenGa1483Controller(m_mockTransport, nullptr);
    m_controller->openDevice();
}

void TestGrattenController::cleanup()
{
    // Called after each test
    delete m_controller;
    delete m_mockTransport;
}

void TestGrattenController::testSetFrequency_Valid()
{
    // Setup: set response for frequency query
    m_mockTransport->setResponse(":FREQuency:CW?", "1000000000");
    m_mockTransport->clearSentCommands();

    // Act: set frequency to 1 GHz
    bool result = m_controller->setFrequency(1, 1e9);

    // Assert: command should have been sent
    QVERIFY(result);

    // Verify that a frequency command was sent
    QStringList sentCommands = m_mockTransport->sentCommands();
    QVERIFY(sentCommands.size() > 0);

    // Check that the command contains the frequency pattern
    bool foundFreqCmd = false;
    for (const QString &cmd : sentCommands) {
        if (cmd.contains(":FREQuency:CW", Qt::CaseInsensitive)) {
            // Should contain 1000000000 or 1e9 in some form
            QVERIFY(cmd.contains("1000000000") || cmd.contains("1e9") || cmd.contains("1000000000"));
            foundFreqCmd = true;
            break;
        }
    }
    QVERIFY(foundFreqCmd);
}

void TestGrattenController::testSetFrequency_OutOfRange()
{
    // Act: try to set frequency outside valid range (too low)
    // Gratten limits are 250kHz to 4GHz
    bool result = m_controller->setFrequency(1, 100e3); // 100 kHz is out of range

    // Assert: should fail due to range check
    QVERIFY(!result);
}

void TestGrattenController::testSetOutput_Enable()
{
    m_mockTransport->setResponse(":OUTPut:STATE?", "1");
    m_mockTransport->clearSentCommands();

    bool result = m_controller->setOutput(1, true);

    QVERIFY(result);

    QStringList sentCommands = m_mockTransport->sentCommands();
    QVERIFY(sentCommands.size() > 0);

    bool foundOutputCmd = false;
    for (const QString &cmd : sentCommands) {
        if (cmd.contains(":OUTPut:STATE", Qt::CaseInsensitive)) {
            QVERIFY(cmd.contains("ON") || cmd.contains("1"));
            foundOutputCmd = true;
            break;
        }
    }
    QVERIFY(foundOutputCmd);
}

void TestGrattenController::testSetOutput_Disable()
{
    m_mockTransport->setResponse(":OUTPut:STATE?", "0");
    m_mockTransport->clearSentCommands();

    bool result = m_controller->setOutput(1, false);

    QVERIFY(result);

    QStringList sentCommands = m_mockTransport->sentCommands();
    QVERIFY(sentCommands.size() > 0);

    bool foundOutputCmd = false;
    for (const QString &cmd : sentCommands) {
        if (cmd.contains(":OUTPut:STATE", Qt::CaseInsensitive)) {
            QVERIFY(cmd.contains("OFF") || cmd.contains("0"));
            foundOutputCmd = true;
            break;
        }
    }
    QVERIFY(foundOutputCmd);
}

void TestGrattenController::testSetAMState_Enable()
{
    m_mockTransport->setResponse(":AM:STATE?", "1");
    m_mockTransport->clearSentCommands();

    bool result = m_controller->setAMState(1, true);

    QVERIFY(result);

    QStringList sentCommands = m_mockTransport->sentCommands();
    QVERIFY(sentCommands.size() > 0);

    bool foundAmStateCmd = false;
    for (const QString &cmd : sentCommands) {
        if (cmd.contains(":AM:STATE", Qt::CaseInsensitive)) {
            QVERIFY(cmd.contains("ON") || cmd.contains("1"));
            foundAmStateCmd = true;
            break;
        }
    }
    QVERIFY(foundAmStateCmd);
}

void TestGrattenController::testSetAMState_Disable()
{
    m_mockTransport->setResponse(":AM:STATE?", "0");
    m_mockTransport->clearSentCommands();

    bool result = m_controller->setAMState(1, false);

    QVERIFY(result);

    QStringList sentCommands = m_mockTransport->sentCommands();
    QVERIFY(sentCommands.size() > 0);

    bool foundAmStateCmd = false;
    for (const QString &cmd : sentCommands) {
        if (cmd.contains(":AM:STATE", Qt::CaseInsensitive)) {
            QVERIFY(cmd.contains("OFF") || cmd.contains("0"));
            foundAmStateCmd = true;
            break;
        }
    }
    QVERIFY(foundAmStateCmd);
}

void TestGrattenController::testSetSweepStart_Valid()
{
    m_mockTransport->setResponse(":SWEep:FREQuency:STARt?", "100000000");
    m_mockTransport->clearSentCommands();

    bool result = m_controller->setSweepStart(1, 1e8);

    QVERIFY(result);

    QStringList sentCommands = m_mockTransport->sentCommands();
    QVERIFY(sentCommands.size() > 0);

    bool foundSweepStartCmd = false;
    for (const QString &cmd : sentCommands) {
        if (cmd.contains(":SWEep:FREQuency:STARt", Qt::CaseInsensitive) ||
            cmd.contains(":SWEep:STARt", Qt::CaseInsensitive)) {
            // Should contain the frequency value
            foundSweepStartCmd = true;
            break;
        }
    }
    QVERIFY(foundSweepStartCmd);
}

void TestGrattenController::testSetAMFrequency_Valid()
{
    m_mockTransport->setResponse(":AM:INTernal:FREQuency?", "5000");
    m_mockTransport->clearSentCommands();

    bool result = m_controller->setAMFrequency(1, 5000);

    QVERIFY(result);

    QStringList sentCommands = m_mockTransport->sentCommands();
    QVERIFY(sentCommands.size() > 0);

    bool foundAmFreqCmd = false;
    for (const QString &cmd : sentCommands) {
        if (cmd.contains(":AM:INTernal:FREQuency", Qt::CaseInsensitive)) {
            foundAmFreqCmd = true;
            break;
        }
    }
    QVERIFY(foundAmFreqCmd);
}

void TestGrattenController::testSetAMDepth_Valid()
{
    m_mockTransport->setResponse(":AM:DEPTh?", "75");
    m_mockTransport->clearSentCommands();

    bool result = m_controller->setAMDepth(1, 75);

    QVERIFY(result);

    QStringList sentCommands = m_mockTransport->sentCommands();
    QVERIFY(sentCommands.size() > 0);

    bool foundAmDepthCmd = false;
    for (const QString &cmd : sentCommands) {
        if (cmd.contains(":AM:DEPTh", Qt::CaseInsensitive)) {
            foundAmDepthCmd = true;
            break;
        }
    }
    QVERIFY(foundAmDepthCmd);
}

void TestGrattenController::testQueryFrequency_Valid()
{
    m_mockTransport->setResponse(":FREQuency:CW?", "2450000000");
    m_mockTransport->clearSentCommands();

    double freq = m_controller->queryFrequency(1);

    QCOMPARE(freq, 2.45e9);

    QStringList cmds = m_mockTransport->sentCommands();
    bool foundQuery = false;
    for (const QString &c : cmds) {
        if (c.contains(":FREQuency:CW?", Qt::CaseInsensitive)) {
            foundQuery = true;
            break;
        }
    }
    QVERIFY(foundQuery);
}

void TestGrattenController::testGetIdentity()
{
    m_mockTransport->setResponse("*IDN?", "Atten,GA1483,SN00001,FW1.0");
    m_mockTransport->clearSentCommands();

    QString idn = m_controller->getIdentity();

    QVERIFY(!idn.isEmpty());
    QVERIFY(idn.contains("GA1483", Qt::CaseInsensitive)
            || idn == "Atten,GA1483,SN00001,FW1.0");
}

void TestGrattenController::testSetPower_Valid()
{
    m_mockTransport->setResponse(":POWer:LEVEL?", "0");
    m_mockTransport->clearSentCommands();

    bool result = m_controller->setAmplitude(1, 0.0, "DBM");

    QVERIFY(result);

    bool foundPowerCmd = false;
    for (const QString &cmd : m_mockTransport->sentCommands()) {
        if (cmd.contains(":POWer:LEVEL", Qt::CaseInsensitive)) {
            foundPowerCmd = true;
            break;
        }
    }
    QVERIFY(foundPowerCmd);
}

void TestGrattenController::testSetPower_OutOfRange()
{
    // GA1483 range: -136 to +13 dBm
    bool result = m_controller->setAmplitude(1, 20.0, "DBM"); // above +13 dBm
    QVERIFY(!result);

    result = m_controller->setAmplitude(1, -200.0, "DBM"); // below -136 dBm
    QVERIFY(!result);
}

QTEST_MAIN(TestGrattenController)
#include "test_gratten_controller.moc"
