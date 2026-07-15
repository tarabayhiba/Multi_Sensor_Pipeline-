cmake_minimum_required(VERSION 3.24)
project(imu_pipeline)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(rosidl_default_generators REQUIRED)

# Generate custom messages
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/ImuReading.msg"
  DEPENDENCIES builtin_interfaces
)

ament_export_dependencies(rosidl_default_runtime)

include_directories(include)

rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}" "rosidl_typesupport_cpp")

add_executable(sensor_driver src/sensor_driver.cpp)
ament_target_dependencies(sensor_driver rclcpp builtin_interfaces)
target_link_libraries(sensor_driver "${cpp_typesupport_target}")

add_executable(filter_node src/filter_node.cpp)
ament_target_dependencies(filter_node rclcpp builtin_interfaces)
target_link_libraries(filter_node "${cpp_typesupport_target}")

add_executable(logger_node src/logger_node.cpp)
ament_target_dependencies(logger_node rclcpp builtin_interfaces)
target_link_libraries(logger_node "${cpp_typesupport_target}")

install(TARGETS
  sensor_driver
  filter_node
  logger_node
  DESTINATION lib/${PROJECT_NAME}
)

# Install launch and config directories
install(DIRECTORY launch config
  DESTINATION share/${PROJECT_NAME}
)

ament_package()