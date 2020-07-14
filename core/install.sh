#!/bin/sh

RED='\033[1;31m'
BLUE='\033[1;36m'
GREEN='\033[1;32m'
NC='\033[0m'
DIR=${PWD}

function fail_on_err {
    if [ $? -ne 0 ]; then
        printf "${RED}$1${NC}\n"
        exit
    fi
}

printf "${BLUE}Building Binary ...${NC}\n"
cd ${DIR}/src
g++ -o ${DIR}/bin/press_core -std=c++11 main.cpp */*.cpp
fail_on_err "Building Failed"
cd ${DIR}

printf "${BLUE}Generating Config ...${NC}\n"
rm ${DIR}/src/config
touch ${DIR}/src/config
fail_on_err "Failed to create config file"
echo ${DIR}/tmp/ >> ${DIR}/src/config
echo ${DIR}/../data/ >> ${DIR}/src/config

printf "${GREEN}Installation Finished${NC}\n"
