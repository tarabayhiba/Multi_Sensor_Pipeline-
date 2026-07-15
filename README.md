# Multi Sensor Pipeline

- `sensor_driver` publishes simulated IMU readings on `/imu/raw` at a adjustable rate
- `filter_node` subscribes to `/imu/raw`, applies a moving average filter & publishes `/imu/filtered`.
- `logger_node` subscribes to both `/imu/raw` & `/imu/filtered` & prints selected values every second.

## Build

```bash
source /opt/ros/jazzy/setup.bash
colcon build --symlink-install
```

## Run

```bash
source /opt/ros/jazzy/setup.bash
source install/setup.bash
ros2 launch imu_pipeline imu_pipeline_launch.py
```

## Verify

```bash
ros2 topic hz /imu/raw
ros2 param list
ros2 param get /imu/sensor_driver publish_rate_hz
```

## Notes

- `logger_node` uses a 1 Hz `rclcpp::WallTimer` to print output not message callbacks
