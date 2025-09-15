#!/bin/sh

SECONDS=0

configs=(
    "build-windows-AXCF2152-2024-LTS-Release build-windows-AXCF2152-2024-LTS-Release"    
    "build-windows-AXCF3152-2024-LTS-Release build-windows-AXCF3152-2024-LTS-Release"
    "windows-VS2022-default build-windows-VS2022-release"
    "linux-default build-linux-Release wsl"
)

for config in "${configs[@]}"; do
    read -r CFG_NAME BUILD_NAME PREFIX <<< "$config"
    ( ${PREFIX:+$PREFIX } cmake -D CFG_NAME:string="$CFG_NAME" -D BUILD_NAME:string="$BUILD_NAME" -P ./misc/bi.cmake ) 2>&1 | ./misc/colorize.sh    
done

duration=$SECONDS
echo -e '\033[1;36m'
echo "$((duration / 60)) minutes and $((duration % 60)) seconds elapsed."
