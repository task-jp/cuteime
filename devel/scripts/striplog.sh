#!/bin/sh
find -name "*.cpp" | xargs sed -i 's/cuteimeDebug/\/\/cuteimeDebug/'
