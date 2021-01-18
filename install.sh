#!/bin/sh

RED='\033[1;31m'
BLUE='\033[1;36m'
GREEN='\033[1;32m'
NC='\033[0m'
DIR=${PWD}
DIR_CORE=${DIR}/core
DIR_APP_SERVER=${DIR}/ui/server

fail_on_err() {
  if [ $? -ne 0 ]; then
    printf "${RED}$1${NC}\n"
    exit
  fi
}

printf "${BLUE}Building PRESS Core ...${NC}\n"
cd ${DIR_CORE}/src
g++ -o ${DIR}/bin/press_core_d -std=c++11 press_core.cpp */*.cpp */*/*.cpp
fail_on_err "Building PRESS Core Failed"
cd ${DIR}

printf "${BLUE}Building PRESS Test Client ...${NC}\n"
cd ${DIR_CORE}/src
g++ -o ${DIR}/bin/press_core_test_client -std=c++11 press_core_test_client.cpp */*.cpp */*/*.cpp
fail_on_err "Building PRESS Test Client Failed"
cd ${DIR}

printf "${BLUE}Building PRESS App Server ...${NC}\n"
cd ${DIR_APP_SERVER}
go build -o ${DIR}/bin/press_app_server_d cmd/*.go
fail_on_err "Building PRESS App Server Failed"
cd ${DIR}

printf "${BLUE}Generating Config File ...${NC}\n"
sed "s+\[ROOT\]+${DIR}+g" ${DIR}/config_template.json > ${DIR}/config.json
fail_on_err "Failed to generate config file"

printf "${BLUE}Downloading Sample Data ...${NC}\n"
cd ${DIR}/data
bash download_data.sh
cd ${DIR}

printf "${GREEN}Installation Finished${NC}\n"
