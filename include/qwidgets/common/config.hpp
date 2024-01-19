#pragma once

#include <QObject>
#include <QColor>

namespace common
{

    class QConfig : public QObject
    {
        Q_OBJECT
    public:
        enum Theme
        {
            LIGHT,
            DARK,
            AUTO,
        };
        Theme theme{DARK};
    signals:
        void appRestartSig();
        void themeColorChanged(QColor);
        void themeChanged(Theme);
    };

    const static QConfig qconfig;

    static bool isDarkTheme()
    {
        return qconfig.theme == QConfig::DARK;
    }
} // namespace name
