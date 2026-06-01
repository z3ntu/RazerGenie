#!/bin/sh
# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: 2022 Luca Weiss <luca@lucaweiss.eu>

# Based on https://gist.github.com/adriansr/1da9b18a8076b0f8a977a5eea0ae41ef

set -e

SIZES="
16,16x16
32,16x16@2x
32,32x32
64,32x32@2x
128,128x128
256,128x128@2x
256,256x256
512,256x256@2x
512,512x512
1024,512x512@2x
"

SVG="$1"
OUT="$2"

BASE=$(basename "$SVG" | sed 's/\.[^\.]*$//')
ICONSET="$BASE.iconset"
mkdir -p "$ICONSET"
for params in $SIZES; do
    size=$(echo "$params" | cut -d, -f1)
    label=$(echo "$params" | cut -d, -f2)
    svg2png -w "$size" -h "$size" "$SVG" "$ICONSET/icon_$label.png"
done

iconutil -c icns -o "$OUT" "$ICONSET"
rm -rf "$ICONSET"
