#ifndef THEMEPROVIDER_H
#define THEMEPROVIDER_H

#include <QPalette>


class ThemeProvider
{
public:
    ThemeProvider();
    static QPalette light(){
        QPalette pal;

        // Backgrounds
        pal.setColor(QPalette::Window,        QColor("#FFFFFF"));
        pal.setColor(QPalette::Base,          QColor("#FFFFFF"));
        pal.setColor(QPalette::AlternateBase, QColor("#F7F7F7"));
        pal.setColor(QPalette::Button,        QColor("#F3F3F3"));

        // Text
        pal.setColor(QPalette::WindowText,    QColor("#000000"));
        pal.setColor(QPalette::Text,          QColor("#000000"));
        pal.setColor(QPalette::ButtonText,    QColor("#000000"));
        pal.setColor(QPalette::PlaceholderText, QColor("#6D6D6D"));
        pal.setColor(QPalette::BrightText,    QColor("#FFFFFF"));

        // Borders
        pal.setColor(QPalette::Light,         QColor("#FFFFFF"));
        pal.setColor(QPalette::Midlight,      QColor("#D9D9D9"));
        pal.setColor(QPalette::Mid,           QColor("#CCCCCC"));
        pal.setColor(QPalette::Dark,          QColor("#A6A6A6"));
        pal.setColor(QPalette::Shadow,        QColor("#D1D1D1"));

        // Selection
        pal.setColor(QPalette::Highlight,     QColor("#CCE8FF"));
        pal.setColor(QPalette::HighlightedText, QColor("#000000"));
        pal.setColor(QPalette::Accent,        QColor("#0078D4"));

        // Links
        pal.setColor(QPalette::Link,          QColor("#005FB8"));
        pal.setColor(QPalette::LinkVisited,   QColor("#7B0796"));

        // pal.setColor(QPalette::Window,           QColor(0xF9F9F9));
        // pal.setColor(QPalette::WindowText,       QColor("#1F1F1F"));
        // pal.setColor(QPalette::Base,             QColor("#FFFFFF"));
        // pal.setColor(QPalette::AlternateBase,    QColor("#F3F3F3"));
        // pal.setColor(QPalette::ToolTipBase,      QColor("#FFFFFF"));
        // pal.setColor(QPalette::ToolTipText,      QColor("#000000"));
        // pal.setColor(QPalette::PlaceholderText,  QColor("#6E6E6E"));
        // pal.setColor(QPalette::Text,             QColor("#000000"));
        // pal.setColor(QPalette::Button,           QColor("#F3F3F3"));
        // pal.setColor(QPalette::ButtonText,       QColor("#000000"));
        // pal.setColor(QPalette::BrightText,       QColor("#FF0000"));
        // pal.setColor(QPalette::Highlight,        QColor("#0078D4"));
        // pal.setColor(QPalette::Accent,           QColor("#0078D4"));
        // pal.setColor(QPalette::HighlightedText,  QColor("#FFFFFF"));
        // pal.setColor(QPalette::Link,             QColor("#0078D4"));
        // pal.setColor(QPalette::LinkVisited,      QColor("#005A9E"));
        // pal.setColor(QPalette::NoRole,           QColor(0, 0, 0, 0));

        return pal;
    };
};

#endif // THEMEPROVIDER_H
