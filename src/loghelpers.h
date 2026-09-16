#ifndef LOGHELPERS_H
#define LOGHELPERS_H

#include <QDebug>
#include <QImage>
#include <QMimeData>
#include <QStringList>
#include <QThread>
#include <QUrl>

inline QString threadTag()
{
    QThread *t = QThread::currentThread();
    const QString name = t->objectName();
    return QStringLiteral("t=%1(0x%2)")
        .arg(name.isEmpty() ? QStringLiteral("?") : name)
        .arg(reinterpret_cast<quintptr>(t), 0, 16);
}

inline QString preview(const QString &s, int max = 120)
{
    QString one = s;
    one.replace('\n', "\\n");
    one.replace('\r', "\\r");
    one.replace('\t', "\\t");
    if (one.size() > max)
        one = one.left(max) + QStringLiteral("...(+%1 more)").arg(s.size() - max);
    return one;
}

inline QString describeMimeData(const QMimeData *md)
{
    if (!md)
        return QStringLiteral("<null mimeData>");

    QStringList fmts;
    for (const QString &f : md->formats())
        fmts << QStringLiteral("%1(%2)").arg(f).arg(md->data(f).size());

    return QStringLiteral("hasUrls=%1 hasImage=%2 hasText=%3 hasHtml=%4 formats=[%5]")
        .arg(md->hasUrls()).arg(md->hasImage())
        .arg(md->hasText()).arg(md->hasHtml())
        .arg(fmts.join(", "));
}

inline void dumpMimeData(const QMimeData *md)
{
    qInfo().noquote() << threadTag() << describeMimeData(md);
    if (!md)
        return;

    for (const QUrl &u : md->urls())
        qInfo().noquote() << threadTag() << "  url:" << u.toString()
                          << "local=" << u.isLocalFile();

    if (md->hasText())
        qInfo().noquote() << threadTag() << "  text:" << preview(md->text());
    if (md->hasHtml())
        qInfo().noquote() << threadTag() << "  html:" << preview(md->html());

    if (md->hasImage()) {
        const QImage img = md->imageData().value<QImage>();
        qInfo().noquote() << threadTag() << "  image:"
            << (img.isNull() ? QStringLiteral("<null>")
                             : QStringLiteral("%1x%2").arg(img.width()).arg(img.height()));
    }
}

#endif // LOGHELPERS_H
