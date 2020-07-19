#!/bin/sh

RED='\033[1;31m'
BLUE='\033[1;36m'
GREEN='\033[1;32m'
NC='\033[0m'

function download_files {
  declare -a fileNames=("${!2}")
  declare -a fileLinks=("${!3}")
  for (( i=0; i<$1; i++ )); do 
    printf "${BLUE}Downloading ${fileNames[$i]}${NC}\n"
    curl -L -o "${fileNames[$i]}" "https://drive.google.com/uc?export=download&id=${fileLinks[$i]}"
  done
}

printf "${GREEN}Downloading Seatle, WA road network and single trajectory ...${NC}\n"
mkdir -p WA_roadnetwork_and_single_trajectory
cd WA_roadnetwork_and_single_trajectory
WAFileNames=("README.md" "road_network.txt" "gps_data.txt" "ground_truth_route.txt")
WAFileLinks=("129PpnEkTxWmZpM0bVyBZXAC7MUSjSOg7" "1_eI6eRWowIHRsZkdd0cvX7fZZX8bAfX0" "1E7wrRffhrLqlXAgB6XOKo2keSdlOqtee" "1pVzMY_8puhpxN7_OlqAHT6Ti_dr5npGh")
WAFileNumber=${#WAFileNames[@]}
download_files WAFileNumber WAFileNames[@] WAFileLinks[@]

