#!/bin/bash
# F1TENTH workspace environment setup
# Usage: source ~/f1tenth_project/setup_env.sh (or use the `f1` alias)

# ROS2 base
source /opt/ros/humble/setup.bash

# Workspace overlay (uses BASH_SOURCE for portability)
source $(dirname "${BASH_SOURCE[0]}")/install/setup.bash

# Detect machine and apply machine-specific settings
if [ -f /etc/nv_tegra_release ]; then
    # Jetson-specific
    export PATH=/usr/local/cuda-12.6/bin:$PATH
    export LD_LIBRARY_PATH=/usr/local/cuda-12.6/lib64:$LD_LIBRARY_PATH
    export VESC_DEV=/dev/sensors/vesc
    MACHINE="Jetson"
else
    # Desktop-specific
    source ~/.venvs/f1tenth/bin/activate
    MACHINE="Desktop"
fi

# Confirmation output
echo "─────────────────────────────────────────"
echo "F1TENTH workspace sourced"
echo "  Machine:    $MACHINE"
echo "  ROS_DISTRO: $ROS_DISTRO"
echo "  Workspace:  $(dirname "${BASH_SOURCE[0]}")"
echo "─────────────────────────────────────────"
