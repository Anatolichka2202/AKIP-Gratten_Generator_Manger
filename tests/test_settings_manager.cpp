#include <QtTest>
#include <QCoreApplication>
#include <QSettings>
#include "settingsmanager.h"

class TestSettingsManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testGrattenHostDefault();
    void testGrattenHostRoundtrip();
    void testGrattenPortDefault();
    void testGrattenPortRoundtrip();
    void testLastDeviceTypeRoundtrip();
    void testLanguageRoundtrip();
    void testTimeoutsDefault();
};

void TestSettingsManager::initTestCase()
{
    // Use isolated test storage so we don't pollute real settings
    QCoreApplication::setOrganizationName("MilTech_Test");
    QCoreApplication::setApplicationName("AKIP_Test");
}

void TestSettingsManager::cleanupTestCase()
{
    // Wipe test settings
    QSettings s(QSettings::IniFormat, QSettings::UserScope,
                 QCoreApplication::organizationName(),
                 QCoreApplication::applicationName());
    s.clear();
    s.sync();
}

void TestSettingsManager::testGrattenHostDefault()
{
    QVERIFY(!SettingsManager::instance().grattenHost().isEmpty());
}

void TestSettingsManager::testGrattenHostRoundtrip()
{
    SettingsManager::instance().setGrattenHost("192.168.1.42");
    QCOMPARE(SettingsManager::instance().grattenHost(), QString("192.168.1.42"));
    SettingsManager::instance().setGrattenHost("192.168.1.1");
}

void TestSettingsManager::testGrattenPortDefault()
{
    quint16 port = SettingsManager::instance().grattenPort();
    QVERIFY(port > 0 && port < 65535);
}

void TestSettingsManager::testGrattenPortRoundtrip()
{
    SettingsManager::instance().setGrattenPort(5555);
    QCOMPARE(SettingsManager::instance().grattenPort(), quint16(5555));
}

void TestSettingsManager::testLastDeviceTypeRoundtrip()
{
    SettingsManager::instance().setLastDeviceType("GRATTEN");
    QCOMPARE(SettingsManager::instance().lastDeviceType(), QString("GRATTEN"));

    SettingsManager::instance().setLastDeviceType("AKIP");
    QCOMPARE(SettingsManager::instance().lastDeviceType(), QString("AKIP"));

    SettingsManager::instance().setLastDeviceType("");
    QCOMPARE(SettingsManager::instance().lastDeviceType(), QString(""));
}

void TestSettingsManager::testLanguageRoundtrip()
{
    SettingsManager::instance().setLanguage("en_US");
    QCOMPARE(SettingsManager::instance().language(), QString("en_US"));

    SettingsManager::instance().setLanguage("ru_RU");
    QCOMPARE(SettingsManager::instance().language(), QString("ru_RU"));
}

void TestSettingsManager::testTimeoutsDefault()
{
    int conn = SettingsManager::instance().grattenConnectTimeoutMs();
    int resp = SettingsManager::instance().grattenResponseTimeoutMs();
    QVERIFY(conn > 0);
    QVERIFY(resp > 0);
}

QTEST_MAIN(TestSettingsManager)
#include "test_settings_manager.moc"
