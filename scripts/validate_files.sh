#!/bin/bash -e
# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: 2018 Luca Weiss <luca@lucaweiss.eu>

echo "Validating json files..."
for file in ./data/matrix_layouts/*.json; do
    echo "Testing file: $file"
    json_verify < "$file"
done
echo

echo "Validating appstream xml..."
appstream-util validate-relax ./data/xyz.z3ntu.razergenie.appdata.xml
