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

        return pal;
    };

    static QPalette dark(){
        QPalette pal;

        // Backgrounds
        pal.setColor(QPalette::Window,        QColor("#202020"));
        pal.setColor(QPalette::Base,          QColor("#1C1C1C"));
        pal.setColor(QPalette::AlternateBase, QColor("#262626"));
        pal.setColor(QPalette::Button,        QColor("#2A2A2A"));
        pal.setColor(QPalette::ToolTipBase,   QColor("#2C2C2C"));

        // Text
        pal.setColor(QPalette::WindowText,    QColor("#FFFFFF"));
        pal.setColor(QPalette::Text,          QColor("#FFFFFF"));
        pal.setColor(QPalette::ButtonText,    QColor("#FFFFFF"));
        pal.setColor(QPalette::BrightText,    QColor("#FFFFFF"));
        pal.setColor(QPalette::ToolTipText,   QColor("#FFFFFF"));
        pal.setColor(QPalette::PlaceholderText, QColor("#7A7A7A"));

        // Borders
        pal.setColor(QPalette::Light,         QColor("#2C2C2C"));
        pal.setColor(QPalette::Midlight,      QColor("#333333"));
        pal.setColor(QPalette::Mid,           QColor("#3A3A3A"));
        pal.setColor(QPalette::Dark,          QColor("#2B2B2B"));
        pal.setColor(QPalette::Shadow,        QColor("#3F3F3F"));   // main border

        // Selection
        pal.setColor(QPalette::Highlight,     QColor("#094771"));
        pal.setColor(QPalette::HighlightedText, QColor("#FFFFFF"));
        pal.setColor(QPalette::Accent,        QColor("#0078D4"));

        // Links
        pal.setColor(QPalette::Link,          QColor("#4DA3FF"));
        pal.setColor(QPalette::LinkVisited,   QColor("#B782FF"));

        return pal;
    }
};

#endif // THEMEPROVIDER_H
