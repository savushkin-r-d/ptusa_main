#!/bin/sh

SECONDS=0

( cmake -D CFG_NAME:string="build-windows-AXCF2152-2022-LTS-Release" -D BUILD_NAME:string="build-windows-AXCF2152-2022-LTS-Release" -P ./misc/bi.cmake ) 2>&1 | ./misc/colorize.sh
( cmake -D CFG_NAME:string="build-windows-AXCF1152-2022-LTS-Release" -D BUILD_NAME:string="build-windows-AXCF1152-2022-LTS-Release" -P ./misc/bi.cmake ) 2>&1 | ./misc/colorize.sh
( cmake -D CFG_NAME:string="windows-VS2022-default" -D BUILD_NAME:string="build-windows-VS2022-release" -P ./misc/bi.cmake ) 2>&1 | ./misc/colorize.sh

duration=$SECONDS
echo -e '\033[1;36m'
echo "$((duration / 60)) minutes and $((duration % 60)) seconds elapsed."
