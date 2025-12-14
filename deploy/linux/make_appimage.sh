#!/usr/bin/env bash

# --- appimagetool ---
if [ ! -x appimagetool-x86_64.AppImage ]; then
    wget -c https://github.com/$(wget -q https://github.com/probonopd/go-appimage/releases/expanded_assets/continuous -O - \
        | grep "appimagetool-.*-x86_64.AppImage" \
        | head -n 1 \
        | cut -d '"' -f 2)
    chmod +x appimagetool-*.AppImage
    mv appimagetool-*.AppImage appimagetool-x86_64.AppImage
fi

# --- deploy deps (Qt, libs) ---
./appimagetool-x86_64.AppImage deploy AppDir/usr/share/applications/oneman.jarusll.Tokri.desktop

# --- build AppImage ---
./appimagetool-x86_64.AppImage "$APPDIR"
