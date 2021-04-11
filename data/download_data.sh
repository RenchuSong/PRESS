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
    curl -L -c mycookie -o temp "https://drive.google.com/uc?export=download&id=${fileLinks[$i]}"
    filesize=$(wc -c temp | awk '{print $1}')
    if [ $filesize -gt 10000 ]; then
      printf "Finish downloading\n"
      mv temp "${fileNames[$i]}.tar.gz"
    else
      content=$(cat temp)
      for (( j=0; j<$filesize-10; j++)); do
        if [ "${content:$j:8}" == "confirm=" ]; then
          for (( k=0; k<10; k++)); do
            if [ "${content:$j+8+$k:1}" == "&" ]; then
              token=${content:$j+8:$k}
            fi
          done
        fi
      done
      printf "Confirm downloading with token ${token}\n"
      curl -L -b mycookie -o "${fileNames[$i]}.tar.gz" "https://drive.google.com/uc?export=download&confirm=${token}&id=${fileLinks[$i]}"
      rm temp
    fi
    rm mycookie
    printf "${WHITE}Creating folder${NC}\n"
    mkdir -p "${fileNames[$i]}"
    printf "${WHITE}Extracting files${NC}\n"
    tar -zxvf "${fileNames[$i]}.tar.gz" -C "${fileNames[$i]}"
    printf "${WHITE}Removing tarball${NC}\n"
    rm "${fileNames[$i]}.tar.gz"
  done
}

FileNames=("SeattleTest")
FileLinks=("1Tekw-ZcqEcnf51tchIKekeHOX7zZw5lR")
FileNumber=${#FileNames[@]}
download_files FileNumber FileNames[@] FileLinks[@]
