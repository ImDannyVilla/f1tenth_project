# VESC Config Changes for Slash 4×4 Ultimate

Applied to: `f1tenth_system/f1tenth_stack/config/vesc.yaml`

## Changes from F1TENTH defaults

| Parameter | Default | Our Value | Reason |
|---|---|---|---|
| speed_to_erpm_gain | 4614.0 | **4124.0** | Recomputed for 2 pole pairs, 11.82 gear, 109.5mm wheel |
| wheelbase | 0.25 | **0.33** | Traxxas Slash 4×4 Ultimate (measured 12⅝ in ≈ 0.32m; rounded to 0.33) |

## Speed-to-ERPM calculation

pole_pairs = 4 poles / 2 = 2
gear_ratio = 11.82 (stock 13T/54T pinion/spur × 3.85 diff)
wheel_circumference = π × 0.1095 = 0.344 m

For 1 m/s ground speed:
  wheel_RPM = 1 / 0.344 × 60 = 174.4
  motor_RPM = 174.4 × 11.82 = 2062
  ERPM = 2062 × 2 = 4124

## Application

After cloning f1tenth_system on a new machine:

```bash
nano ~/f1tenth_project/src/f1tenth_system/f1tenth_stack/config/vesc.yaml
# Change speed_to_erpm_gain to 4124.0
# Change wheelbase to 0.33
# Save
```

Then rebuild with `colcon build --packages-select f1tenth_stack`.

## VESC firmware side (separate — via VESC Tool, not ROS)

Motor Settings → Additional Info → Setup:
- motor_poles: 14 → **4**
- gear_ratio: 3.000 → **11.82**
- wheel_diameter: 83mm → **109.5mm**
- battery_type: LI-ION → **LIPO**
- battery_capacity: 6.0 Ah → **5.2 Ah**

Then Motor Write, then re-run FOC detection.
