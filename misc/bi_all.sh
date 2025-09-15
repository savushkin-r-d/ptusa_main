#!/bin/bash

SECONDS=0

configs=(
    "build-windows-AXCF2152-2024-LTS-Release build-windows-AXCF2152-2024-LTS-Release AXCF2152"
    "build-windows-AXCF3152-2024-LTS-Release build-windows-AXCF3152-2024-LTS-Release AXCF3152"
    "windows-VS2022-default build-windows-VS2022-release windows"
    "linux-default build-linux-Release linux wsl"
)

build_times=()
tmp_dir="/tmp/bi_all_times_$$"
mkdir -p "$tmp_dir"

pids=()
for i in "${!configs[@]}"; do
    config="${configs[$i]}"
    read -r CFG_NAME BUILD_NAME SHORT_NAME PREFIX <<< "$config"
    (
        start_time=$(date +%s)
        {
            echo "Начало сборки: $CFG_NAME"
            ${PREFIX:+$PREFIX } cmake -D CFG_NAME:string="$CFG_NAME" -D BUILD_NAME:string="$BUILD_NAME" -P ./misc/bi.cmake
            echo "Завершение сборки: $CFG_NAME"
        } | while IFS= read -r line; do echo "$((i+1))> [$SHORT_NAME]  $line"; done
        end_time=$(date +%s)
        duration=$((end_time - start_time))
        echo "$duration" > "$tmp_dir/build_time_$i.txt"
    ) 2>&1 | ./misc/colorize.sh &
    pids+=($!)
done

for pid in "${pids[@]}"; do
    wait $pid
done

echo -e '\033[1;36m'
echo "Время сборки по конфигурациям:"
for i in "${!configs[@]}"; do
    config="${configs[$i]}"
    read -r CFG_NAME BUILD_NAME SHORT_NAME PREFIX <<< "$config"
    if [[ -f "$tmp_dir/build_time_$i.txt" ]]; then
        duration=$(cat "$tmp_dir/build_time_$i.txt")
        echo "$((i+1))> [$SHORT_NAME] $CFG_NAME: $duration секунд"
        rm -f "$tmp_dir/build_time_$i.txt"
    fi
done
rmdir "$tmp_dir"

duration=$SECONDS
echo -e "\033[1;36mОбщее время: $duration секунд ($((duration / 60)):$((duration % 60))).\033[0m"
