#!/bin/sh

[ ! -f ./main ] && ./build.sh
./main > output.ppm

