#!/bin/bash

SECONDS=0

configs=(
    "build-windows-AXCF2152-2024-LTS-Release build-windows-AXCF2152-2024-LTS-Release AXCF2152"
    "build-windows-AXCF3152-2024-LTS-Release build-windows-AXCF3152-2024-LTS-Release AXCF3152"
    "windows-VS2022-default build-windows-VS2022-release windows"
    "linux-default build-linux-Release linux wsl"
)

pids=()
for i in "${!configs[@]}"; do
    config="${configs[$i]}"
    read -r CFG_NAME BUILD_NAME SHORT_NAME PREFIX <<< "$config"
    (
        {
            echo "Начало сборки: $CFG_NAME"
            ${PREFIX:+$PREFIX } cmake -D CFG_NAME:string="$CFG_NAME" -D BUILD_NAME:string="$BUILD_NAME" -P ./misc/bi.cmake
            echo "Завершение сборки: $CFG_NAME"
        } | while IFS= read -r line; do echo "$((i+1))> [$SHORT_NAME]  $line"; done
    ) 2>&1 | ./misc/colorize.sh &
    pids+=($!)
done

for pid in "${pids[@]}"; do
    wait $pid
done

duration=$SECONDS
echo -e '\033[1;36m'
echo "$((duration / 60)) минут и $((duration % 60)) секунд затрачено."
