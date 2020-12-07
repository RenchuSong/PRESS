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
g++ -o ${DIR}/bin/press_core_d -std=c++11 press_core.cpp */*.cpp */*/*.cpp
fail_on_err "Building PRESS Core Failed"

g++ -o ${DIR}/bin/press_core_test_client -std=c++11 press_core_test_client.cpp */*.cpp */*/*.cpp

cd ${DIR}

sed "s+\[ROOT\]+${DIR}+g" ${DIR}/config_template.json > ${DIR}/config.json

rm PRESS_CORE_IN
mkfifo PRESS_CORE_IN
rm PRESS_CORE_OUT
mkfifo PRESS_CORE_OUT
printf "${BLUE}Running PRESS Core ...${NC}\n"
./bin/press_core_d ${DIR}/config.json ${DIR}/PRESS_CORE_IN ${DIR}/PRESS_CORE_OUT
