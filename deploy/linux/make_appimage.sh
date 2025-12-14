#!/usr/bin/env bash
set -e

# appimagetool
if [ ! -x appimagetool-x86_64.AppImage ]; then
    wget -c https://github.com/$(wget -q https://github.com/probonopd/go-appimage/releases/expanded_assets/continuous -O - \
        | grep "appimagetool-.*-x86_64.AppImage" \
        | head -n 1 \
        | cut -d '"' -f 2)
    chmod +x appimagetool-*.AppImage
    mv appimagetool-*.AppImage appimagetool-x86_64.AppImage
fi

export QTDIR=/usr/lib/x86_64-linux-gnu/qt6
export QT_PLUGIN_PATH=$QTDIR/plugins
export QT_QPA_PLATFORM_PLUGIN_PATH=$QTDIR/plugins/platforms

# deploy
./appimagetool-x86_64.AppImage --appimage-extract
./squashfs-root/AppRun -s deploy ./AppDir/usr/share/applications/oneman.jarusll.Tokri.desktop

echo "AppImage created successfully."

# # build
./squashfs-root/AppRun ./AppDir
