# safety_node — "node doesn't appear in `ros2 node list`" diagnosis & fix

## Symptom

`ros2 run safety_node safety_node` started, the process stayed **alive** (18 threads,
sleeping on a futex), produced **no log output**, and never showed up in
`ros2 node list`. `ros2 topic info /drive` reported **Publisher count: 0**.

## Root cause (a bug in the code, not the build)

```cpp
// main()
rclcpp::executors::MultiThreadedExecutor executor;
executor.add_node(std::make_shared<Safety>());   // ❌ node destroyed immediately
executor.spin();
```

`rclcpp::Executor::add_node()` stores only a **`weak_ptr`** to the node — it does
**not** take ownership. `std::make_shared<Safety>()` here is a **temporary**
`shared_ptr` and the only strong reference. The moment that statement ends (`;`),
the temporary is destroyed, the refcount hits 0, and the `Safety` node is
**destructed right away** — tearing down its DDS participant, the `/drive`
publisher, and both subscriptions.

`executor.spin()` then runs against an expired weak_ptr and just blocks forever
doing nothing. That explains every symptom:

| Observation | Why |
|---|---|
| Process alive, 18 threads | rclcpp context/DDS threads survive the node |
| No callbacks / no `Speed` logs | node object is gone, nothing is subscribed |
| `/drive` Publisher count: 0 | publisher was destroyed with the node |
| Invisible in `ros2 node list` | node's DDS entities were never kept alive |

## The fix

Keep a named `shared_ptr` alive for the whole lifetime of `spin()`:

```cpp
rclcpp::executors::MultiThreadedExecutor executor;
auto node = std::make_shared<Safety>();   // ✅ strong reference stays alive
executor.add_node(node);
executor.spin();
```

Equivalent shorthand for the single-node case (the temporary lives as a function
argument for the whole call):

```cpp
rclcpp::spin(std::make_shared<Safety>());
```

The footgun is specifically `add_node(temporary)` followed by a *separate* `spin()`.

## How it was tracked down

1. `ros2 pkg executables safety_node` → the node **was** built/installed.
   (`pkg executables` = "what can run"; `node list` = "what *is* running now".)
2. A stock `talker` launched from the same shell **was** visible → DDS/discovery
   is healthy, so the problem is specific to this node.
3. `ros2 topic info /drive` showed **Publisher count: 0** while the process was
   alive → the node built its DDS context but never registered its entities.
4. Temporary `std::cerr` checkpoints in `main()`/constructor accidentally *fixed*
   it — because the instrumented version introduced a named `node` variable. That
   pinpointed the weak_ptr lifetime bug.

## Two side notes

- **Nested `build/install/log`**: caused by running `colcon build` *inside*
  `src/safety_node/`. They were stale clutter (removed). Always run `colcon build`
  from the workspace root (`~/f1tenth_ws`).
- **Launch file**: not required. `ros2 run safety_node safety_node` is enough for a
  single node. Launch files are for starting multiple nodes / passing params.

## Mental model

`ros2 node list` shows only nodes that are **running right now**. Building a node
never makes it appear — something has to be alive and spinning.
