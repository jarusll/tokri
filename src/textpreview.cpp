#include "textpreview.h"

#include <QFile>
#include <QFontMetrics>
#include <QPainter>
#include <QStringDecoder>
#include <QStringList>

namespace {

constexpr int kProbeBytes = 8192;
constexpr int kMaxLines = 10;
constexpr int kOuterPad = 4;
constexpr int kInnerPad = 8;
constexpr int kRadius = 8;

QString decodeText(const QByteArray &bytes)
{
    QStringDecoder decoder(QStringDecoder::Utf8);
    const QString text = decoder(bytes);
    if (decoder.hasError())
        return QString::fromLatin1(bytes);
    return text;
}

QStringList toLines(const QString &text)
{
    QStringList lines = text.split(QLatin1Char('\n'));
    for (QString &line : lines) {
        if (line.endsWith(QLatin1Char('\r')))
            line.chop(1);
        line.replace(QLatin1Char('\t'), QLatin1String("    "));
    }
    return lines;
}

}

bool isTextLike(const QMimeType &mime)
{
    return mime.inherits(QLatin1String("text/plain"));
}

QImage renderTextPreview(const QString &path, const QSize &target,
                         const PreviewStyle &style)
{
    if (target.isEmpty())
        return {};

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return {};

    const QByteArray bytes = file.read(kProbeBytes);
    if (bytes.isEmpty() || bytes.contains('\0'))
        return {};

    QStringList lines = toLines(decodeText(bytes));
    while (!lines.isEmpty() && lines.last().trimmed().isEmpty())
        lines.removeLast();
    if (lines.isEmpty())
        return {};

    const QSize device(target.width() * style.dpr,
                       target.height() * style.dpr);
    QImage image(device, QImage::Format_ARGB32_Premultiplied);
    image.setDevicePixelRatio(style.dpr);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    const QRect bounds(QPoint(0, 0), target);
    const QRect background =
        bounds.adjusted(kOuterPad, kOuterPad, -kOuterPad, -kOuterPad);

    painter.setPen(style.border);
    painter.setBrush(style.background);
    painter.drawRoundedRect(background, kRadius, kRadius);

    const QRect textRect =
        background.adjusted(kInnerPad, kInnerPad, -kInnerPad, -kInnerPad);

    QFont font = style.font;
    font.setPixelSize(qMax(textRect.height() / (kMaxLines + 2), 4));
    painter.setFont(font);

    const QFontMetrics fm(font);
    const int lineHeight = fm.height();

    painter.setPen(style.foreground);
    painter.setClipRect(textRect);

    int y = textRect.top() + fm.ascent();
    for (int i = 0; i < lines.size() && i < kMaxLines; ++i) {
        if (y > textRect.bottom())
            break;

        const QString line =
            fm.elidedText(lines.at(i), Qt::ElideRight, textRect.width());
        painter.drawText(textRect.left(), y, line);
        y += lineHeight;
    }

    return image;
}
