#!/bin/bash
cd "$(dirname "$0")"
pandoc -s --template template.html --metadata title="ps1 dvd logo demo" readme.md -o readme.html
