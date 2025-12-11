#ifndef THEMEPROVIDER_H
#define THEMEPROVIDER_H

#include <QPalette>


class ThemeProvider
{
public:
    ThemeProvider();
    static QPalette light(){
        QPalette pal;
        pal.setColor(QPalette::Window,           QColor("#F9F9F9"));
        pal.setColor(QPalette::WindowText,       QColor("#1F1F1F"));
        pal.setColor(QPalette::Base,             QColor("#FFFFFF"));
        pal.setColor(QPalette::AlternateBase,    QColor("#F3F3F3"));
        pal.setColor(QPalette::ToolTipBase,      QColor("#FFFFFF"));
        pal.setColor(QPalette::ToolTipText,      QColor("#000000"));
        pal.setColor(QPalette::PlaceholderText,  QColor("#6E6E6E"));
        pal.setColor(QPalette::Text,             QColor("#000000"));
        pal.setColor(QPalette::Button,           QColor("#F3F3F3"));
        pal.setColor(QPalette::ButtonText,       QColor("#000000"));
        pal.setColor(QPalette::BrightText,       QColor("#FF0000"));
        pal.setColor(QPalette::Highlight,        QColor("#0078D4"));
        pal.setColor(QPalette::Accent,           QColor("#0078D4"));
        pal.setColor(QPalette::HighlightedText,  QColor("#FFFFFF"));
        pal.setColor(QPalette::Link,             QColor("#0078D4"));
        pal.setColor(QPalette::LinkVisited,      QColor("#005A9E"));
        pal.setColor(QPalette::NoRole,           QColor(0, 0, 0, 0));

        return pal;
    };
};

#endif // THEMEPROVIDER_H
