#include "settingsmanager.h"

SettingsManager& SettingsManager::instance()
{
    static SettingsManager s;
    return s;
}

SettingsManager::SettingsManager()
    : m_settings("TestLab", "AKIP-Gratten-Manager")
{}

// --- Gratten LAN ---

QString SettingsManager::grattenHost() const
{
    return m_settings.value("gratten/host", "192.168.1.66").toString();
}

void SettingsManager::setGrattenHost(const QString &host)
{
    m_settings.setValue("gratten/host", host);
}

quint16 SettingsManager::grattenPort() const
{
    return static_cast<quint16>(m_settings.value("gratten/port", 5025).toUInt());
}

void SettingsManager::setGrattenPort(quint16 port)
{
    m_settings.setValue("gratten/port", port);
}

int SettingsManager::grattenConnectTimeoutMs() const
{
    return m_settings.value("gratten/connectTimeoutMs", 5000).toInt();
}

void SettingsManager::setGrattenConnectTimeoutMs(int ms)
{
    m_settings.setValue("gratten/connectTimeoutMs", ms);
}

int SettingsManager::grattenResponseTimeoutMs() const
{
    return m_settings.value("gratten/responseTimeoutMs", 3000).toInt();
}

void SettingsManager::setGrattenResponseTimeoutMs(int ms)
{
    m_settings.setValue("gratten/responseTimeoutMs", ms);
}

// --- General ---

QString SettingsManager::language() const
{
    return m_settings.value("general/language", "ru").toString();
}

void SettingsManager::setLanguage(const QString &lang)
{
    m_settings.setValue("general/language", lang);
}

QString SettingsManager::lastDeviceType() const
{
    return m_settings.value("general/lastDeviceType", "").toString();
}

void SettingsManager::setLastDeviceType(const QString &type)
{
    m_settings.setValue("general/lastDeviceType", type);
}
