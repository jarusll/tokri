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
cp ../../build/Tokri "$APPDIR/usr/bin/$APP"
chmod +x "$APPDIR/usr/bin/$APP"

# desktop
cp ./oneman.jarusll.Tokri.desktop "$APPDIR/usr/share/applications/"

# icon (minimal)
cp ./Tokri.png "$APPDIR/$APP.png"

