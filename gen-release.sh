#!/bin/bash

if [ ! -d ""$(dirname "$0")"/build" ]; then
    echo "You need to build ps1 dvd logo demo first"
    return 1
fi
VER=1.0
cd "$(dirname "$0")"/build
rm -rf ps1-dvd-logo-demo-build-$VER ../ps1-dvd-logo-demo-build-$VER.zip
mkdir ps1-dvd-logo-demo-build-$VER
cp -rv ../readme.md ../readme.html ../license.txt dvd-logo-demo.exe dvd-logo-demo.cue dvd-logo-demo.bin ps1-dvd-logo-demo-build-$VER
chmod -R 777 ps1-dvd-logo-demo-build-$VER
zip -r ../ps1-dvd-logo-demo-build-$VER.zip ps1-dvd-logo-demo-build-$VER
rm -rf ps1-dvd-logo-demo-build-$VER
