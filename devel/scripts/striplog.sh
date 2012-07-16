#!/bin/sh
find -name "*.cpp" | xargs sed -i 's/qimsysDebug/\/\/qimsysDebug/'
