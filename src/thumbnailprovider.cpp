#include "thumbnailprovider.h"

#include <QPixmap>
#include <QPainter>
#include <QGuiApplication>
#include <QPalette>

ThumbnailProvider::ThumbnailProvider(QObject *parent)
    : QObject{parent}
{}

QString ThumbnailProvider::makeKey(const QFileInfo &fi) const
{
    return fi.filePath() + "|" +
           QString::number(fi.lastModified().toMSecsSinceEpoch());
}

QIcon ThumbnailProvider::iconForFile(const QFileInfo &fi, const QSize &size) const
{
    const QString key =
        fi.filePath() + "|" +
        QString::number(fi.lastModified().toMSecsSinceEpoch());

    if (auto *cached = cache.object(key)) {
        return *cached;
    }

    QIcon icon;

    const auto mime = db.mimeTypeForFile(fi);
    if (mime.name().startsWith("image/")) {
        QPixmap pm(fi.filePath());
        if (!pm.isNull()) {
            pm = pm.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            icon = QIcon(pm);
        }
    } else if (mime.name().startsWith("text/")) {
        icon = textPreviewIcon(fi, size);
    }

    if (icon.isNull())
        icon = iconProvider.icon(fi);

    cache.insert(key, new QIcon(icon), 1);
    return icon;
}

QIcon ThumbnailProvider::textPreviewIcon(const QFileInfo &fi,
                                         const QSize &size) const
{
    QFile f(fi.filePath());
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    QTextStream ts(&f);
    QString text = ts.read(60).simplified();
    if (text.isEmpty())
        return {};

    QPixmap pm(size);
    pm.fill(Qt::transparent);

    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);

    constexpr int outerPad = 4;
    constexpr int innerPad = 8;
    constexpr int radius  = 8;

    const QPalette pal = QGuiApplication::palette();

    QRect bgRect = pm.rect().adjusted(outerPad, outerPad,
                                      -outerPad, -outerPad);

    p.setPen(pal.color(QPalette::Mid));
    p.setBrush(pal.color(QPalette::Button));
    p.drawRoundedRect(bgRect, radius, radius);

    QRect textRect = bgRect.adjusted(innerPad, innerPad,
                                     -innerPad, -innerPad);

    QFont font = p.font();
    font.setPointSizeF(font.pointSizeF() * 0.9);
    p.setFont(font);
    p.setPen(pal.color(QPalette::Text));

    p.drawText(textRect,
               Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
               text);

    return QIcon(pm);
}
