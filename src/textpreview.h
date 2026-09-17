#ifndef TEXTPREVIEW_H
#define TEXTPREVIEW_H

#include <QColor>
#include <QFont>
#include <QImage>
#include <QMimeType>
#include <QSize>
#include <QString>

struct PreviewStyle
{
    QFont font;
    QColor background;
    QColor border;
    QColor foreground;
    qreal dpr = 1.0;
};

bool isTextLike(const QMimeType &mime);

QImage renderTextPreview(const QString &path, const QSize &target,
                         const PreviewStyle &style);

#endif // TEXTPREVIEW_H
