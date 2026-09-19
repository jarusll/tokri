#ifndef WINDOWICONS_H
#define WINDOWICONS_H

class QApplication;
class QWidget;

namespace WindowIcons {

// Applies the platform's window-icon policy.
//
// Icons are inherently platform-specific:
//   Windows: the title bar (ICON_SMALL) and the taskbar/Alt+Tab entry
//            (ICON_BIG) are separate images and Qt's setWindowIcon() cannot
//            tell them apart, so both are set through WM_SETICON.
//   macOS:   no-op; the icon comes from the bundle's Tokri.icns.
//   Linux:   Qt's normal application icon (.desktop + installed PNG).
//
void apply(QApplication &app, QWidget &window);

} // namespace WindowIcons

#endif // WINDOWICONS_H
