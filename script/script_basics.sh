#!/bin/bash

set -euo pipefail

PLOT_PATH="plot"
PLOT_DATA_PATH="data"

LOG_PATH="log"
BUILD_LOG_NAME="build.txt"
RUN_LOG_NAME="run.txt"

JSON_FILE="config.json"

clear(){
    rm -rf $1
    mkdir $1
}