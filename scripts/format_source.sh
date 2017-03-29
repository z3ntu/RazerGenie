#!/bin/bash

find src/ -name "*.cpp" -o -name "*.h" | xargs astyle --style=linux -n
