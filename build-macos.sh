#!/bin/bash
set -e

# ---- setup ----
APP_NAME="Tokri"
BUILD_DIR="build/Release"
APP="${BUILD_DIR}/${APP_NAME}.app"
OUT_DIR="dist"

DMG_ROOT="$OUT_DIR/dmgroot"
DMG_PATH="$OUT_DIR/${APP_NAME}.dmg"

echo "▶ setup"
rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

# ---- deploy ----
echo "▶ macdeployqt"
macdeployqt "$APP"

# ---- dmg ----
echo "▶ dmg"
mkdir -p "$DMG_ROOT"
cp -R "$APP" "$DMG_ROOT/"
ln -s /Applications "$DMG_ROOT/Applications"

hdiutil create \
  -volname "$APP_NAME" \
  -srcfolder "$DMG_ROOT" \
  -ov -format UDZO \
  "$DMG_PATH"

# ---- teardown ----
rm -rf "$DMG_ROOT"

echo "▶ done"
ls -lh "$DMG_PATH"
