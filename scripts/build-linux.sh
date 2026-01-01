#!/usr/bin/env bash
set -euo pipefail

APP_ID="net.surajyadav.Tokri"
MANIFEST="${APP_ID}.yml"
BUILD_DIR="build-flatpak"
REPO_DIR="repo"
BUNDLE="../dist/${APP_ID}.flatpak"
BRANCH="stable"

rm -rf "${BUILD_DIR}" "${REPO_DIR}" "${BUNDLE}"

flatpak-builder \
  --force-clean \
  --default-branch="${BRANCH}" \
  --repo="${REPO_DIR}" \
  "${BUILD_DIR}" \
  "${MANIFEST}"

flatpak build-bundle \
  "${REPO_DIR}" \
  "${BUNDLE}" \
  "${APP_ID}" \
  "${BRANCH}"
