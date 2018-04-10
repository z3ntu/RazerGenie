#!/bin/bash -e

echo "Validating json files..."
for file in ./data/matrix_layouts/*.json; do
    echo "Testing file: $file"
    json_verify < $file
done
echo

echo "Validating appstream xml..."
appstream-util validate-relax ./data/xyz.z3ntu.razergenie.appdata.xml
