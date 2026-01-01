#include "thumbnailprovider.h"

#include <QPixmap>
#include <QPainter>
#include <QGuiApplication>
#include <QPalette>
#include <QtGui/qimagereader.h>
#include <QtGui/qpainterpath.h>

namespace {
    constexpr int outerPad = 4;
    constexpr int innerPad = 8;
    constexpr int radius   = 8;
    constexpr int maxDim = 256;
}

ThumbnailProvider::ThumbnailProvider(QObject *parent)
    : QObject{parent}
{}

QString ThumbnailProvider::makeKey(const QFileInfo &fi) const
{
    return fi.filePath() + "|" +
           QString::number(fi.lastModified().toMSecsSinceEpoch());
}

QIcon ThumbnailProvider::iconForFile(const QFileInfo &fi,
                                     const QSize &size) const
{
    if (!fi.exists() || fi.size() == 0)
        return iconProvider.icon(fi);

    const QString key = makeKey(fi);

    if (auto *cached = cache.object(key))
        return *cached;

    const QMimeType mime =
        db.mimeTypeForFile(fi, QMimeDatabase::MatchContent);

    QIcon icon;

    if (mime.name().startsWith("image/")) {
        icon = imagePreviewIcon(fi, size);
    } else if (mime.name().startsWith("text/")) {
        icon = textPreviewIcon(fi, size);
    }

    if (icon.isNull())
        return iconProvider.icon(fi);

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

QIcon ThumbnailProvider::imagePreviewIcon(const QFileInfo &fi,
                                          const QSize &size) const
{
    QPixmap src(fi.filePath());
    if (src.isNull())
        return {};

    QSize target = size;
    target.scale(maxDim, maxDim, Qt::KeepAspectRatio);

    QPixmap pm(target);
    pm.fill(Qt::transparent);

    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);

    const QRect r = pm.rect();

    QPainterPath clip;
    clip.addRoundedRect(r, radius, radius);
    p.setClipPath(clip);

    QPixmap scaled =
        src.scaled(r.size(),
                   Qt::KeepAspectRatio,
                   Qt::SmoothTransformation);

    const QPoint topLeft(
        r.center().x() - scaled.width()  / 2,
        r.center().y() - scaled.height() / 2);

    p.drawPixmap(topLeft, scaled);

    return QIcon(pm);
}
