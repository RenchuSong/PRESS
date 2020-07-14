#!/bin/sh

RED='\033[1;31m'
BLUE='\033[1;36m'
GREEN='\033[1;32m'
NC='\033[0m'
DIR=${PWD}
DIR_CORE=${DIR}/core

function fail_on_err {
  if [ $? -ne 0 ]; then
    printf "${RED}$1${NC}\n"
    exit
  fi
}

printf "${BLUE}Building PRESS Core ...${NC}\n"
cd ${DIR_CORE}/src
g++ -o ${DIR}/bin/press_core -std=c++11 main.cpp */*.cpp
fail_on_err "Building PRESS Core Failed"
cd ${DIR}

printf "${BLUE}Generating PRESS Core Config ...${NC}\n"
rm ${DIR_CORE}/src/config
touch ${DIR_CORE}/src/config
fail_on_err "Failed to create PRESS Core config file"
echo ${DIR_CORE}/tmp/ >> ${DIR_CORE}/src/config
echo ${DIR}/data/ >> ${DIR_CORE}/src/config

printf "${GREEN}Installation Finished${NC}\n"
