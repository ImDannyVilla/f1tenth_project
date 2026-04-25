# F1TENTH Autonomous Racing - Perception Engineering

## Overview
Self-study project targeting AV perception engineering roles (Waymo, Aurora, Cruise).
Built on the F1TENTH platform with emphasis on sensor fusion, real-time perception, and ROS2.

## Hardware Stack
- Jetson Orin NX 16GB
- Hokuyo UST-10LX LiDAR
- ZED 2i Stereo Camera + IMU
- VESC 6 MkV1
- Traxxas Slash 4x4

## Software Stack
- Ubuntu 22.04, ROS2 Humble
- FAST-LIO2 (planned)
- nuScenes 3D detection (planned)

## Setup
git clone -b dev-humble https://github.com/f1tenth/f1tenth_gym_ros.git src/f1tenth_gym_ros
cd src/f1tenth_gym_ros
git clone -b dev-humble https://github.com/f1tenth/f1tenth_gym.git
pip install -e src/f1tenth_gym_ros/f1tenth_gym
colcon build

