#!/bin/bash
set -e

# ---- setup ----
APP_NAME="Tokri"
BUILD_DIR="build/Release"
APP="${BUILD_DIR}/${APP_NAME}.app"
OUT_DIR="dist"
ZIP_PATH="$OUT_DIR/${APP_NAME}-macOS.zip"

echo "▶ setup"
rm -f "$ZIP_PATH"
mkdir -p "$OUT_DIR"

# ---- deploy ----
echo "▶ macdeployqt"
macdeployqt "$APP"

# ---- package ----
echo "▶ packaging"
ditto -c -k --keepParent "$APP" "$ZIP_PATH"

# ---- teardown ----
echo "▶ done"
ls -lh "$ZIP_PATH"
