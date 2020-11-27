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

kill_existing_press() {
    while true; do 
        pid="$(ps aux | grep press_core_d | grep -v 'grep' | cut -d' ' -f2- | xargs | cut -d' ' -f1)"
        if [ "${pid}" = '' ];
        then
            break
        else
            kill ${pid}
            fail_on_err "Failed to stop PRESS Core."
        fi
    done
}

option="${1}"
case ${option} in
    "start")
        kill_existing_press
        rm PRESS_CORE_IN  > /dev/null 2>&1
        mkfifo PRESS_CORE_IN
        fail_on_err "Failed to create inbound channel for PRESS Core."
        rm PRESS_CORE_OUT > /dev/null 2>&1
        mkfifo PRESS_CORE_OUT
        fail_on_err "Failed to create outbound channel for PRESS Core."
        ${DIR}/bin/press_core_d ${DIR}/config.json ${DIR}/PRESS_CORE_IN ${DIR}/PRESS_CORE_OUT
        fail_on_err "Failed to start PRESS Core."
        printf "${BLUE}PRESS Core started ...${NC}\n"
        ;;
    "stop")
        kill_existing_press
        printf "${BLUE}PRESS Core stopped ...${NC}\n"
        ;;
    *)
        echo "`basename ${0}`: usage: [start] | [stop]"
        exit 1
        ;;
esac
