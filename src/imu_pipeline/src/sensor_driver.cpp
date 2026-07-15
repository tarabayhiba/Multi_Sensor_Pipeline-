#include <memory>

#include "imu_pipeline/sensor_driver.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SensorDriver>());
  rclcpp::shutdown();
  return 0;
}
