#!/bin/bash

export PATH="$PATH:$(pwd)/script"
export PATH="usr/local/share/npm/bin:$PATH"
export TASK2_PROJ_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export TASK2_SCRIPT=$TASK2_PROJ_DIR/script
export GTK_PATH="/usr/lib/gtk-3.0"