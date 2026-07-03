# F1TENTH Driver Stack Comparison: `f1tenth_system` (Foxy) vs GIU-F1Tenth Fork

**Analysis date:** 2026-07-02
**Analyzed on:** Ubuntu 22.04 x86_64, ROS2 Humble, rosdep 0.26.0
**Target:** Jetson Orin NX 16GB, Ubuntu 22.04, ROS2 Humble, ARM64
**Repos analyzed (read-only):**
- Original: `~/f1tenth_ws/src/f1tenth_system_foxy/` (github.com/f1tenth/f1tenth_system)
- GIU fork: `~/f1tenth_ws/f1t_system_maybe_humble/giu_f1t_system/` (github.com/GIU-F1Tenth/giu_f1t_system, HEAD `52445e2`)

---

## Executive Summary

**Neither repo works out of the box, but the original `f1tenth_system` is far closer to a viable Humble bringup — use it, with two targeted changes.** The GIU fork's headline value ("Humble compatibility") reduces to exactly two deltas: (1) its `vesc` submodule is pinned to the head of the upstream `f1tenth/vesc` **`humble` branch** (commit `1bc8251`, Feb 2025), and (2) its launch uses `urg_node2` as a lifecycle node. Everything else in the fork is a liability for your use case: its `bringup_launch.py` has grown from a 141-line driver bringup into a 605-line race-stack launcher referencing **~20 packages private to the GIU team** (`pure_pursuit`, `gap_follower`, `decision`, `control_gateway`, `MPC_karim`, `kayn_controller`, …) that are not in the repo and not on any package index — and it contains a fatal typo (`ld.add_action(dwa_node)/joy_node`, line 584) that raises `TypeError` the moment the launch file executes, meaning **the fork's bringup cannot run at all as committed**. Confidence: high for the fork's non-viability (verified by direct inspection and Python semantics); high for the original+patches path (all dependencies verified resolvable, though final proof is a build on the Jetson).

A second finding that changes the picture: **`urg_node` is NOT Foxy-only — `ros-humble-urg-node` exists in apt** (verified via `apt-cache` and `rosdep resolve`). It's `urg_node2` that has **no** Humble apt package or rosdep rule (source-build only, from Hokuyo-aut's GitHub). So the original stack's LiDAR path is *easier* to satisfy on Humble than the fork's.

---

## Structural Differences

- **Original**: 1 ROS package (`f1tenth_stack`) + 3 git submodules (`vesc`, `ackermann_mux`, `teleop_tools`), all pinned to Foxy-era branches/commits (2021). Submodules are **uninitialized** in the local clone.
- **GIU fork**: 2 ROS packages (`f1tenth_stack`, `joystick_converter`) + the same 3 submodules (also uninitialized locally). Repo root is nested: `f1t_system_maybe_humble/giu_f1t_system/`.
- **`.gitmodules` diff**: identical except `vesc` tracks branch `humble` in the fork vs `foxy` in the original. `ackermann_mux` and `teleop_tools` track `foxy-devel` in both.
- **GIU-only additions**: `f1tenth_stack/config_sim/`, `maps/`, `path/` directories; 24 extra config YAMLs; `bringup_launch_sim.py` and `bringup_launch_test.py`; `joystick_converter` package (3 nodes, empty `package.xml` — declares zero runtime deps it actually has, e.g. `rclpy`, `sensor_msgs`).
- **`f1tenth_stack/package.xml` is byte-identical in both repos** — the fork never updated its manifest, so it declares `urg_node` while its launch uses `urg_node2`. Stale manifest.
- Both `setup.py` files correctly install `launch/` and `config/` via `data_files`; all 25 config files referenced by the GIU launch do exist in-repo (configs are not the blocker; missing *packages* are).

## Submodule Pin Comparison (Task 4)

Commit dates fetched from the GitHub API (local submodules uninitialized; source dirs untouched per constraints).

| Submodule | Original pin | GIU pin | Newer? | Notes |
|---|---|---|---|---|
| `f1tenth/vesc` | `1952e79` (2021-08-12, `foxy`) | `1bc8251` (2025-02-18) | **Yes — GIU** | GIU pin == current head of upstream `humble` branch. This is the correct pin for Humble. |
| `f1tenth/ackermann_mux` | `6fcf96f` (2021-09-23) | `b3c0b08` (2021-11-10) | Yes — GIU | GIU pin == current head of `foxy-devel`. **No humble branch exists upstream.** |
| `f1tenth/teleop_tools` | `d6fd091` (2021-09-04) | `4337558` (2022-01-26) | Yes — GIU | Upstream has `humble-devel` (`163827a`) that *neither* repo uses. Moot: `ros-humble-teleop-tools` / `ros-humble-joy-teleop` are in apt. |

## Dependency Comparison Table (Task 2)

Declared in `package.xml`:

| Dependency | Original `f1tenth_stack` | GIU `f1tenth_stack` | GIU `joystick_converter` |
|---|---|---|---|
| ackermann_msgs | depend | depend | — |
| urg_node | depend | depend (stale — launch uses urg_node2) | — |
| joy | depend | depend | — |
| joy_teleop | depend | depend | — |
| rosbridge_server | depend | depend | — |
| vesc | depend | depend | — |
| ackermann_mux | depend | depend | — |
| std_msgs | depend | depend | — |
| ament_copyright / ament_flake8 / ament_pep257 / python3-pytest | test_depend | test_depend | test_depend |

**Undeclared, launch-implied deps (GIU `bringup_launch.py` only):** `urg_node2`, `nav2_map_server`, `nav2_amcl`, `nav2_lifecycle_manager`, `robot_localization`, `tf2_ros`, plus ~19 private GIU packages: `pure_pursuit`, `trajectory_planning`, `gap_follower`, `trailing_controller`, `control_gateway`, `decision`, `obj_detection`, `horizon_mapper`, `overtaking`, `kayn_controller`, `lqr_controller`, `state_publisher`, `MPC_karim`, `lidar_filter`, `overtaking_spline`, `mcl_particle_filter` (test launch), `artemis_imu_ros2` (test launch), and more.

## Humble Availability Table

Verified with `apt-cache search --names-only '^ros-humble-<dep>$'` and `rosdep resolve <key> --rosdistro humble` on this x86_64 machine. **Assumption:** the ROS 2 apt repo builds the same package set for arm64/jammy as amd64/jammy — true for all core released packages, but re-verify on the Jetson with the same `apt-cache` command (requires manual check).

| Dependency | Humble status |
|---|---|
| ackermann_msgs | **apt** (`ros-humble-ackermann-msgs`) |
| urg_node | **apt** (`ros-humble-urg-node`) |
| urg_node2 | **need source** (no apt pkg, no rosdep rule; clone github.com/Hokuyo-aut/urg_node2) |
| joy | **apt** (`ros-humble-joy`) |
| joy_teleop | **apt** (`ros-humble-joy-teleop`) |
| teleop_tools | **apt** (`ros-humble-teleop-tools`) — submodule unnecessary |
| rosbridge_server | **apt** (`ros-humble-rosbridge-server`) |
| std_msgs | **apt** (`ros-humble-std-msgs`) |
| vesc (+vesc_driver, vesc_ackermann, vesc_msgs) | **need source** (no apt pkg, no rosdep rule; use submodule @ `humble` branch) |
| ackermann_mux | **need source** (no apt pkg, no rosdep rule; use submodule @ `foxy-devel` head) |
| nav2_map_server / nav2_amcl / nav2_lifecycle_manager | **apt** (GIU launch only) |
| robot_localization | **apt** (GIU launch only) |
| ~19 GIU private packages | **unavailable anywhere public** — blocker for GIU launch |

## rosdep Simulation — GIU fork (Task 5)

Run: `rosdep install --from-paths src --ignore-src -r --simulate --rosdistro humble` on a copy at `/tmp/rosdep_test` (log: `/tmp/rosdep_output.log`). Note the repo actually lives at `~/f1tenth_ws/f1t_system_maybe_humble/` (the `~/f1t_system_maybe_humble/` path in the task description didn't exist); copy path adjusted accordingly.

Would install via apt: `ros-humble-urg-node`, `ros-humble-joy-teleop`, `ros-humble-rosbridge-server` (ackermann_msgs, joy, std_msgs already installed on this desktop, hence omitted from the install list).

Cannot resolve: `ackermann_mux` — **and also `vesc`**: rosdep's error output lists only one unresolved key per package; an isolated re-test (`/tmp/rosdep_min`) confirmed `vesc` has no rosdep rule for Humble either. Don't trust the single-error output. `urg_node2` never appears because the stale `package.xml` doesn't declare it — the simulation *understates* the fork's real requirements.

## ROS2 API Compatibility Scan (Task 3)

- **No hardcoded distro paths**: zero matches for `/opt/ros/foxy|galactic|humble` or `ROS_DISTRO` checks in any `.py`, `CMakeLists.txt`, `.sh`, `.xml`, `.yaml` in either repo (submodules excluded — uninitialized).
- **No CMakeLists.txt at all** in either repo's own packages — all are `ament_python`. (C++ concerns live in the `vesc` submodule, which is exactly why the `humble` branch pin matters: Humble tightened `declare_parameter` and other rclcpp APIs vs Foxy.)
- **Launch API**: both import `IncludeLaunchDescription` + `XMLLaunchDescriptionSource` but never call them (dead imports — harmless). No deprecated launch patterns. GIU's lifecycle handling (`LifecycleNode`, `OnStateTransition`, `ChangeState`, `matches_action`) is the correct modern Humble idiom, borrowed from upstream `urg_node2`'s own launch file — worth stealing if you go the urg_node2 route.
- **Python 3.10**: every `.py` in both repos byte-compiles clean under Python 3.10.12 (`py_compile`, 0 failures). No removed-API usage (`collections.Mapping`, `asyncio.coroutine`, `distutils`, etc.).
- **`os.path` vs `pathlib`**: both use `os.path.join` in launch files and `setup.py`. This remains the dominant idiom in Humble-era launch files (including upstream ROS packages) — style note only, not a compatibility issue.
- **GIU fatal runtime bug**: `bringup_launch.py:584` — `ld.add_action(dwa_node)/joy_node`. Parses as division of `None` by a `Node` object → `TypeError` at launch. The launch file cannot execute.

## Compatibility Blockers

**Original `f1tenth_system` on Humble:**
1. `vesc` submodule pinned to Foxy-era commit `1952e79` on the `foxy` branch — Foxy-era rclcpp C++ will not build cleanly on Humble. Fix: use the `humble` branch (`1bc8251`).
2. `teleop_tools` submodule pinned to `foxy-devel` — building it from source would shadow/conflict with apt. Fix: don't init it; apt provides `joy_teleop`.
3. `ackermann_mux` has no Humble release or branch — must build `foxy-devel` head from source (expected to build: it's a simple C++ mux; verify on Jetson).
4. Submodules currently uninitialized in your clone — nothing builds until `git submodule update --init`.

**GIU fork on Humble:**
1. **`bringup_launch.py:584` `TypeError`** — launch crashes unconditionally.
2. **~19 private packages** launched (many unconditionally: `trailing_controller`, `decision` fsm, `obj_detection`, `control_gateway`, `trajectory_planning`, `state_publisher`, `robot_localization` w/ their ekf.yaml, `lidar_filter`, `overtaking_spline`, …) that do not exist in the repo or any public index → launch fails even after fixing (1).
3. `urg_node2` required by launch, available only via source build; not declared in `package.xml`, so rosdep will never surface it.
4. Stale `package.xml` (still `urg_node`), `joystick_converter` declares zero of its real deps — manifests can't be trusted for dependency resolution.
5. Configs (`vesc.yaml`: `speed_to_erpm_gain: 3050`, `wheelbase: 0.33`, `servo 0.0–1.0`, `speed_max: 100000`) are tuned for *their* car, not your Slash 4×4 Ultimate — silently wrong values, some safety-relevant (servo limits, 100k ERPM cap).

## Recommended Path Forward

**"Use original + these forks/patches":** clone upstream `f1tenth/f1tenth_system` on the Jetson and apply:

1. Switch the `vesc` submodule to the `humble` branch head `1bc8251` (the one thing the GIU fork got right).
2. Skip the `teleop_tools` submodule; `sudo apt install ros-humble-joy-teleop ros-humble-teleop-tools`.
3. Init `ackermann_mux` at its `foxy-devel` head and build from source.
4. Use `ros-humble-urg-node` from apt with the stock `sensors.yaml` (already set to `192.168.0.10:10940`, `laser` frame — matches your UST-10LX). **Fallback** if scans misbehave: build `urg_node2` from source and graft in the GIU/upstream lifecycle-launch block.
5. Keep the stock `vesc.yaml` as the starting point and apply *your own* measured values (your VESC audit: 4-pole, 11.82:1, 109.5 mm) — do not copy GIU's tuning.

Do **not** base the bringup on the GIU fork. If you want its `urg_node2` lifecycle snippet or `params_ether.yaml` later, copy those two files; nothing else transfers.

## Specific Fixes (exact changes)

1. **`f1tenth_system/.gitmodules`** — change `[submodule "vesc"]` → `branch = foxy` to `branch = humble`; then `git submodule update --init vesc && git -C vesc checkout 1bc8251` (or `git submodule set-branch --branch humble vesc && git submodule update --init --remote vesc`).
2. **Do not run** `git submodule update --init teleop_tools` (or delete its entry from `.gitmodules`) — use apt instead. If you init everything by accident, `touch teleop_tools/COLCON_IGNORE`.
3. **`f1tenth_stack/package.xml`** — no change required (`urg_node` is valid on Humble). If you switch to urg_node2: replace `<depend>urg_node</depend>` with `<depend>urg_node2</depend>` and clone `https://github.com/Hokuyo-aut/urg_node2` (recursive — it vendors `urg_library`) into the workspace.
4. **`f1tenth_stack/config/vesc.yaml`** — after motor re-test, set your corrected values (erpm gain must be recomputed for 4 poles × 11.82 ratio × 109.5 mm wheel; stock `4614.0` assumes the ERPM/(m/s) of the reference build).
5. *(Only if you ever revive the GIU launch)* **`giu_f1t_system/f1tenth_stack/launch/bringup_launch.py:584`** — `ld.add_action(dwa_node)/joy_node` → `ld.add_action(dwa_node)`, then delete or condition every node whose package you don't have. Not recommended.

## Assumptions Made

- apt availability checked on **x86_64**; arm64/jammy parity assumed (standard for the ROS 2 apt repo) — re-run the `apt-cache` checks on the Jetson to confirm.
- Submodule build-ability on Humble (`vesc@humble` builds; `ackermann_mux@foxy-devel` builds) is inferred from branch naming and code vintage, not compiled here (per your no-build constraint) — the colcon build on the Jetson is the final test.
- Task 5's original copy path `~/f1t_system_maybe_humble/` did not exist; used `~/f1tenth_ws/f1t_system_maybe_humble/` per your "adjust paths" instruction.
- Your Task 5 message was truncated mid-command; I used the standard `rosdep install --from-paths src --ignore-src -r --simulate` you started to specify, per the `mkdir/cp` commands you provided.
- GIU submodule pin dates came from the GitHub API (submodules uninitialized locally; fetching them would have modified the source dirs you asked me not to touch).
