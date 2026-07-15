import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import SetEnvironmentVariable

def generate_launch_description():
    pkg_share = get_package_share_directory('imu_pipeline')
    config_file = os.path.join(pkg_share, 'config', 'imu_pipeline.yaml')

    sensor_driver_node = Node(
        package='imu_pipeline',
        executable='sensor_driver',
        name='sensor_driver',
        namespace='imu',
        parameters=[config_file],
        remappings=[('imu_raw', 'raw')],
        output='screen',
    )

    filter_node = Node(
        package='imu_pipeline',
        executable='filter_node',
        name='filter_node',
        namespace='imu',
        parameters=[config_file],
        remappings=[('imu_raw', 'raw'), ('imu_filtered', 'filtered')],
        output='screen',
    )

    logger_node = Node(
        package='imu_pipeline',
        executable='logger_node',
        name='logger_node',
        namespace='imu',
        parameters=[config_file],
        remappings=[('imu_raw', 'raw'), ('imu_filtered', 'filtered')],
        output='screen',
    )

    SetEnvironmentVariable(
            name='RCUTILS_CONSOLE_OUTPUT_FORMAT',
            value='[{severity} {date_time_with_ms}] [{name}]: {message}'),

    return LaunchDescription([
        sensor_driver_node,
        filter_node,
        logger_node,
    ])
