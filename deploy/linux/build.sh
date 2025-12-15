#!/bin/sh
set -e

APP_ID=oneman.jarusll.Tokri
MANIFEST=$APP_ID.yml
BUILD_DIR=build-flatpak

flatpak-builder \
  --force-clean \
  --user \
  --install \
  "$BUILD_DIR" \
  "$MANIFEST"
