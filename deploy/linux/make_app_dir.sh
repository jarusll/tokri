#!/usr/bin/env bash
set -e

APP=Tokri
APPDIR=AppDir

# clean
rm -rf "$APPDIR"

# dirs
mkdir -p "$APPDIR/usr/bin"
mkdir -p "$APPDIR/usr/share/applications"

# binary
cp ./build/Tokri "$APPDIR/usr/bin/$APP"
chmod +x "$APPDIR/usr/bin/$APP"

# desktop
cp ./Tokri.desktop "$APPDIR/usr/share/applications/"

# icon (minimal)
cp ./Tokri.png "$APPDIR/$APP.png"

# AppRun
cat > "$APPDIR/AppRun" <<'EOF'
#!/bin/sh
HERE="$(dirname "$(readlink -f "$0")")"

export LD_LIBRARY_PATH="$HERE/usr/lib:$LD_LIBRARY_PATH"
export QT_PLUGIN_PATH="$HERE/usr/lib/qt6/plugins"
export QML2_IMPORT_PATH="$HERE/usr/lib/qt6/qml"

exec "$HERE/usr/bin/Tokri" "$@"
EOF
chmod +x "$APPDIR/AppRun"

echo "AppDir ready: $APPDIR"

