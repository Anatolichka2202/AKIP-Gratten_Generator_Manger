#pragma once
#include <QApplication>
#include <QFile>
#include <QString>

namespace ThemeManager {

enum class Theme { Dark, Light };

inline void apply(QApplication &app, Theme theme)
{
    QString common, themeFile;
    QFile f1(":/styles/design/style/common.qss");
    if (f1.open(QFile::ReadOnly | QFile::Text))
        common = f1.readAll();

    QString path = (theme == Theme::Dark)
        ? ":/styles/design/style/dark.qss"
        : ":/styles/design/style/light.qss";

    QFile f2(path);
    if (f2.open(QFile::ReadOnly | QFile::Text))
        themeFile = f2.readAll();

    app.setStyleSheet(common + "\n" + themeFile);
}

} // namespace ThemeManager
