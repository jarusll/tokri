#ifndef THUMBNAILLAYOUT_H
#define THUMBNAILLAYOUT_H

#include <Qt>

namespace ThumbnailLayout {

// Square side of the icon rendered by the native icon-mode delegate.
inline constexpr int Container = 128;

// Number of caption lines shown below the thumbnail before eliding.
inline constexpr int CaptionLines = 3;

// Tile dimensions (height depends on the caption font).
inline constexpr int TileWidth = Container;
inline constexpr int TileHeight(int fontHeight)
{
    return 4 + Container + 6 + CaptionLines * fontHeight + 4;
}

}

#endif // THUMBNAILLAYOUT_H
