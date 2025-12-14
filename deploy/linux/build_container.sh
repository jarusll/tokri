#!/bin/sh
set -e

IMAGE=tokri-appimage-builder

docker build --pull -t $IMAGE -f Containerfile .
