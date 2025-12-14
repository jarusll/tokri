#!/bin/sh
set -e

IMAGE=tokri-appimage-builder
SRC="$HOME/source/Droppables"

docker run --rm -it \
  -v "$SRC:/source" \
  -w /source \
  $IMAGE \
  /bin/sh <<'EOF'
set -e

rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

cd deploy/linux
rm -rf AppDir
./make_app_dir.sh
./make_appimage.sh

EOF
