#!/bin/bash

find src/ -name "*.cpp" -o -name "*.h" -o -name "*.qdoc" | xargs astyle --style=linux -n
