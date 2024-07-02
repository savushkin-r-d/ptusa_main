#!/bin/sh

# -D CFG_NAME:string="build-windows-AXCF1152-2022-LTS-Release" -D BUILD_NAME:string="build-windows-AXCF1152-2022-LTS-Release"
# -D CFG_NAME:string="windows-VS2022-default" -D BUILD_NAME:string="build-windows-VS2022-release"

( cmake -D CFG_NAME:string="build-windows-AXCF2152-2022-LTS-Release" -D BUILD_NAME:string="build-windows-AXCF2152-2022-LTS-Release" -P ./misc/bi.cmake ) 2>&1 | awk -v IGNORECASE=1 '
    /( failed)|( not found)|( warning)|( could not find)/   {print "\033[33m" $0 "\033[39m"; next} # Yellow
    /( found)|( find)/ && /(no )/                           {print "\033[33m" $0 "\033[39m"; next} # Yellow
    /( success)|( found)|( done)/ && !/(no )|(nothing )/    {print "\033[32m" $0 "\033[39m"; next} # Green
    / debug/                                                {print "\033[37m" $0 "\033[39m"; next} # Grey
    1 {print "\033[37m" $0 "\033[39m"}
    '
