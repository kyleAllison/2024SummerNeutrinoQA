#!/usr/bin/bash

SHINE_DIR=/afs/cern.ch/user/n/na61qa/workDirectory/2024NeutrinoRun/Shine

source $SHINE_DIR/Shine_src/Tools/Scripts/env/lxplus.sh x86_64-el9-gcc12-opt
eval $($SHINE_DIR/Shine_install/bin/shine-offline-config --env-sh)
