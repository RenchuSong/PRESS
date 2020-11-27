#!/bin/sh

RED='\033[1;31m'
BLUE='\033[1;36m'
GREEN='\033[1;32m'
NC='\033[0m'
DIR=${PWD}
DIR_CORE=${DIR}/core

fail_on_err() {
    if [ $? -ne 0 ]; then
        printf "${RED}$1${NC}\n"
        exit
    fi
}

check_press_core() {
    pid="$(ps aux | grep press_core_d | grep -v 'grep' | cut -d' ' -f2- | xargs | cut -d' ' -f1)"
    if [ "${pid}" = '' ];
    then
        printf "${RED}Please run './padmin.sh start' first!${NC}\n"
        exit
    fi
}

check_press_core
printf "${BLUE}Start running PRESS test client ...${NC}\n"
${DIR}/bin/press_core_test_client ${DIR}/PRESS_CORE_OUT ${DIR}/PRESS_CORE_IN
fail_on_err "Failed to run PRESS test client."
printf "${BLUE}Finished running PRESS test client ...${NC}\n"
