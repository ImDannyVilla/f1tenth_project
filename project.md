# F1TENTH Autonomous Racing Project

## Project Overview
**Objective**: Build a complete autonomous racing system optimized for learning AV perception engineering skills, with emphasis on real-time performance, sensor fusion, and production-ready practices.

**Target Outcome**: A working F1TENTH car that autonomously navigates racing scenarios while demonstrating industry-relevant perception capabilities. Portfolio piece for AV perception engineering roles (Waymo, Aurora, Cruise) and agricultural autonomy (Carbon Robotics, with professor connection).

## Key Success Metrics
- Autonomous lap completion on indoor F1TENTH-scale track
- Real-time perception pipeline (<50ms end-to-end latency)
- Multi-sensor fusion via FAST-LIVO2 (LiDAR + camera + IMU)
- Professional-grade documentation and code quality
- Clear connection to production AV applications

## Technical Focus Areas
1. **Perception**: LiDAR + vision fusion (FAST-LIVO2), real-time 3D object detection
2. **Safety**: Collision avoidance (AEB via iTTC), teleop mux with deadman's switch
3. **Performance**: Real-time optimization, algorithm efficiency, latency profiling
4. **Integration**: ROS2 Humble, hardware-software interface, cross-machine deployment
5. **Portfolio**: Industry-relevant documentation, transferable skills demonstration

## Current Status

**Phase**: Phase 2 — Hardware Integration & Testing (in progress)
- [x] Phase 1: Project Setup & Environment
- [ ] Phase 2: Hardware Integration & Testing (~60% complete)
- [ ] Phase 3: Basic Perception Pipeline
- [ ] Phase 4: Sensor Fusion Implementation (FAST-LIO2 → FAST-LIVO2)
- [ ] Phase 5: Racing Logic & Control
- [ ] Phase 6: Safety & Optimization
- [ ] Phase 7: Documentation & Portfolio

**Current Priority**: F1TENTH driver stack selection on Jetson. Evaluating GIU-F1Tenth fork vs. individual driver installs for ROS2 Humble compatibility. Comparison of both stacks staged on desktop for analysis before Jetson deployment.

**Recent Accomplishments**:
- Complete Jetson Orin NX bringup: JetPack 6.2.2, CUDA 12.6 toolkit, ROS2 Humble, ZED SDK 5.3
- Multi-device SSH/Tailscale workflow (Mac → devbox/Jetson) with MagicDNS
- Custom Ghostty terminfo installed on Jetson for SSH terminal parity
- All sensors individually validated on hardware:
  - Hokuyo UST-10LX: Ethernet configured, NetworkManager static IP profile, ping successful
  - ZED 2i: USB 3.0 enumerated, SDK installed, camera visible in ZED Explorer
  - VESC 6 MkVI: FOC motor detection complete, PPM servo output enabled
- udev rules in place for persistent device naming (VESC, PS4 joystick)
- Lab 2 (Safety Node / AEB) working in simulation on desktop
- VESC configuration audit: identified errors in F1TENTH reference XML (motor poles 14→4, gear ratio 3.0→11.82, wheel diameter 83→109.5mm, battery type LI-ION→LIPO)
- PS4 controller working via USB; Logitech F710 ordered for wireless (Jetson kernel lacks hid-playstation and uinput modules for Bluetooth)

**Immediate Next Steps**:
1. Analyze GIU vs upstream F1TENTH stacks on desktop (compatibility report)
2. Based on analysis: either clone GIU fork to Jetson OR fall back to individual driver install
3. Push safety_node code to GitHub; deploy from sim to real Jetson hardware
4. Complete VESC parameter corrections (poles=4, gear ratio=11.82, wheel diameter=109.5mm, LIPO type)
5. End-to-end teleop validation: joystick → drive command → motor spin + servo turn
6. First real /scan visualization in RViz over Tailscale

## Technical Specifications

**Hardware**:
- Chassis: Traxxas Slash 4×4 Ultimate (stock 13T/54T gearing, 11.82 final drive ratio)
- Motor: Traxxas Velineon 3500 brushless (4-pole, 3500 Kv)
- Motor controller: Trampa VESC 6 MkVI
- 2D LiDAR: Hokuyo UST-10LX (Ethernet, 40Hz, 270° FOV, 10m range)
- Stereo Camera + IMU: Stereolabs ZED 2i (USB 3.0, 2K/1080p/720p, 6-axis IMU)
- On-vehicle compute: NVIDIA Jetson Orin NX 16GB w/ NVMe SSD (~100 TOPS AI perf)
- Controller: Sony DualShock 4 (USB working), Logitech F710 incoming (wireless)
- Battery: 3S LiPo, 5200mAh
- Incoming: Livox Mid-360S 3D LiDAR (FAST-LIVO2 fusion research)

**Software Stack**:
- OS: Ubuntu 22.04 LTS (desktop + Jetson matched)
- Framework: ROS2 Humble Hawksbill
- Perception SDK: ZED SDK 5.3
- Languages: C++ (perception/control performance-critical), Python (launch files, config, tooling)
- Dev environment: CLion (desktop), CLI + Ghostty (Jetson), JetBrains Gateway for remote
- Simulation: f1tenth_gym_ros + ROS2 bridge (dev-humble branch)
- Infrastructure: Tailscale VPN mesh, SSH key auth, GitHub, git submodules

**Compute Comparison**:
- devbox (danny-macheen): Ubuntu 22.04 x86_64, RTX 4070, 64GB RAM (sim + heavy dev)
- Jetson (jetson-orinNX): ARM64, integrated Ampere GPU, 16GB RAM (on-car compute)

## Timeline & Constraints

**Project Duration**: Multi-semester (CS junior through senior year at CSU Bakersfield)

**Key Milestones**:
- Real /scan → ROS2 flowing: Q3 2026
- Full bringup working (teleop → drive): Q3 2026
- FAST-LIO2 on public dataset: Q4 2026
- FAST-LIVO2 with Mid-360S: Q1 2027
- LLNL Data Science Challenge (parallel commitment): July 20-31, 2026

**Weekly Time Commitment**: ~10-15 hours (variable with coursework)

## Learning Goals

**Primary Skills to Develop**:
- Real-time perception algorithms (iTTC, LiDAR clustering, VIO)
- Multi-sensor fusion (LiDAR-visual-inertial via FAST-LIVO2)
- ROS2 system architecture (topics, launch orchestration, mux)
- Performance optimization (CUDA, TensorRT deployment on ARM64)
- Professional software practices (git, CI, cross-machine deployment, containers)
- Systems debugging (kernel modules, USB enumeration, network stack, Bluetooth stack)

**Industry Connections**:
- Sensor abstraction pattern parallels production AV stacks (Waymo, Cruise, Aurora)
- ARM64 deployment pipeline directly relevant to on-vehicle compute
- FAST-LIVO2 fusion demonstrates state-of-the-art perception research
- Multi-machine dev workflow reflects production robotics engineering
- Real-time constraint awareness maps to safety-critical systems

## Deliverables
- Working autonomous racing system (real hardware, not just sim)
- Technical documentation with AV industry connections
- Performance analysis and latency profiling report
- Video demonstrations
- Open-source codebase with professional commit history
- Architecture documentation for portfolio/interviews

## Current Challenges & Questions

**Active**:
- F1TENTH stack Foxy → Humble migration (evaluating GIU fork vs individual install)
- VESC XML has wrong metadata for Slash 4×4 Ultimate; manual corrections pending motor re-test

**Resolved**:
- Jetson bring-up: complete flash + full software stack
- Hokuyo Ethernet on non-standard interface name (`enP8p1s0`)
- PS4 controller Bluetooth: confirmed unworkable on stripped NVIDIA kernel; USB fallback + F710 ordered
- Multi-machine SSH workflow with Tailscale MagicDNS

## Resources & References

**Documentation**:
- F1TENTH docs: https://f1tenth.readthedocs.io/en/foxy_test/
- ROS2 Humble docs: https://docs.ros.org/en/humble/
- ZED SDK docs: https://www.stereolabs.com/docs/
- FAST-LIVO2 (2024) LiDAR-inertial-visual odometry paper

**Community**:
- GIU-F1Tenth GitHub org (active Humble-focused fork)
- F1TENTH community discussions
- CSU Bakersfield UAS/Robotics club

## Notes for Claude
- Focus on practical, implementable advice with concrete commands
- Emphasize real-time performance considerations
- Connect solutions to industry practices where meaningful
- Help with both technical implementation and professional development framing
- Be honest about dead ends and pivot rather than sink time
- Prefer teaching underlying concepts over just giving commands when learning value is high
- Direct answers over hedging when I'm asking direct questions
