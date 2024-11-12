#!/bin/bash

CUR_PATH=$(pwd)

ICELL_HOME=${CUR_PATH}
export ICELL_HOME

ICELL_LIC_SERVER_BIN=${ICELL_HOME}/bin
ICELL_LIC_SERVER_DATA=${ICELL_HOME}/data
ICELL_LIC_SERVER_LIB=${ICELL_HOME}/lib
ICELL_LIC_SERVER_LIB64=${ICELL_HOME}/lib64

export ICELL_LIC_SERVER_DATA

PATH=${PATH}:${ICELL_LIC_SERVER_BIN}
LD_LIBRARY_PATH=${ICELL_LIC_SERVER_LIB}:${LD_LIBRARY_PATH}

if [ ! -f "${ICELL_LIC_SERVER_DATA}/license.dat" ]; then
    echo "Error: license.dat not found. Exiting..."
fi