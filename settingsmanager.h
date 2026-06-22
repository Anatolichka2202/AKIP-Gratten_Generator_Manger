#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QSettings>
#include <QString>

class SettingsManager
{
public:
    static SettingsManager& instance();

    // Gratten LAN connection
    QString grattenHost() const;
    void setGrattenHost(const QString &host);
    quint16 grattenPort() const;
    void setGrattenPort(quint16 port);
    int grattenConnectTimeoutMs() const;
    void setGrattenConnectTimeoutMs(int ms);
    int grattenResponseTimeoutMs() const;
    void setGrattenResponseTimeoutMs(int ms);

    // General
    QString language() const;
    void setLanguage(const QString &lang);
    QString lastDeviceType() const;
    void setLastDeviceType(const QString &type);

private:
    SettingsManager();
    ~SettingsManager() = default;
    SettingsManager(const SettingsManager &) = delete;
    SettingsManager &operator=(const SettingsManager &) = delete;

    QSettings m_settings;
};

#endif // SETTINGSMANAGER_H
