#!/bin/bash
set -e

cd ~/f1tenth_ws
source setup_env.sh

ACTIVE_PKG="${1:-safety_node}"

colcon build --cmake-args -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

source install/setup.bash

if [ -f "build/${ACTIVE_PKG}/compile_commands.json" ]; then
    ln -sf "build/${ACTIVE_PKG}/compile_commands.json" compile_commands.json
    echo "✓ compile_commands.json -> build/${ACTIVE_PKG}/compile_commands.json"
else
    echo "⚠ No compile_commands.json found for package '${ACTIVE_PKG}'"
    echo "  Available packages:"
    ls build/ 2>/dev/null | grep -v COLCON_IGNORE | sed 's/^/    /'
    exit 1
fi