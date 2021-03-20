#!/bin/sh

RED='\033[1;31m'
BLUE='\033[1;36m'
GREEN='\033[1;32m'
WHITE='\033[1;37m'
NC='\033[0m'

function download_files {
  declare -a fileNames=("${!2}")
  declare -a fileLinks=("${!3}")
  for (( i=0; i<$1; i++ )); do 
    printf "${WHITE}Downloading ${fileNames[$i]}${NC}\n"
    curl -L -o "${fileNames[$i]}.tar.gz" "https://drive.google.com/uc?export=download&id=${fileLinks[$i]}"
    mkdir -p "${fileNames[$i]}"
    tar -zxvf "${fileNames[$i]}.tar.gz" -C "${fileNames[$i]}"
    rm "${fileNames[$i]}.tar.gz"
  done
}

FileNames=("WA_roadnetwork_and_single_trajectory")
FileLinks=("1zOZPp4ztgjKXFCaxfWu4_50BFRFk3Ocd")
FileNumber=${#FileNames[@]}
download_files FileNumber FileNames[@] FileLinks[@]
