#ifndef THUMBNAILLAYOUT_H
#define THUMBNAILLAYOUT_H

#include <Qt>

namespace ThumbnailLayout {

// A tile is one list-view cell: the thumbnail plus its caption below.
// The container is the square box that holds the framed thumbnail image.

// Square side of the container (margin box).
inline constexpr int Container = 128;

// CSS box model, from the container edge inward.
inline constexpr int Margin = 4;       // space outside the border
inline constexpr int BorderWidth = 1;  // white frame stroke
inline constexpr int Padding = 4;      // space between border and content
inline constexpr int BorderRadius = 8;

// Caption element below the thumbnail.
inline constexpr int CaptionMargin = 6;   // gap between thumbnail and caption
inline constexpr int CaptionPadding = 4;  // space below the caption

// The actual thumbnail image inside margin + border + padding.
inline constexpr int ContentSide =
    Container - 2 * (Margin + BorderWidth + Padding);

// Tile dimensions (height depends on the caption font).
inline constexpr int TileWidth = Container;
inline constexpr int TileHeight(int fontHeight)
{
    return Margin + Container + CaptionMargin + fontHeight + CaptionPadding;
}

// Role carrying the already-sized QPixmap from the proxy model.
inline constexpr int ThumbnailPixmapRole = Qt::UserRole + 100;

}

#endif // THUMBNAILLAYOUT_H
