#pragma once

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "imu_pipeline/msg/imu_reading.hpp"

using namespace std::chrono_literals;

class SensorDriver : public rclcpp::Node
{
public:
  SensorDriver() : Node("sensor_driver")
  {
    this->declare_parameter<double>("publish_rate_hz", 50.0);
    const double rate_hz = this->get_parameter("publish_rate_hz").as_double();

    publisher_ = this->create_publisher<imu_pipeline::msg::ImuReading>("imu_raw", 20);

    const auto period = std::chrono::duration<double>(1.0 / rate_hz);
    timer_ = this->create_wall_timer(
      std::chrono::duration_cast<std::chrono::nanoseconds>(period),
      std::bind(&SensorDriver::publish_reading, this));

    start_time_ = this->now();

    RCLCPP_INFO(this->get_logger(), "sensor_driver publishing at %.2f Hz", rate_hz);
  }

private:
  void publish_reading()
  {
    auto msg = imu_pipeline::msg::ImuReading();
    const double t = (this->now() - start_time_).seconds();

    // Sinusoid on accel axes, phase-shifted per axis so they visibly differ.
    msg.accel_x = std::sin(t);
    msg.accel_y = std::sin(t + 2.0);
    msg.accel_z = std::sin(t + 4.0);

    // Random noise on gyro axes.
    msg.gyro_x = (static_cast<double>(std::rand()) / RAND_MAX - 0.5) * 0.2;
    msg.gyro_y = (static_cast<double>(std::rand()) / RAND_MAX - 0.5) * 0.2;
    msg.gyro_z = (static_cast<double>(std::rand()) / RAND_MAX - 0.5) * 0.2;

    msg.stamp = this->now();

    publisher_->publish(msg);
  }

  rclcpp::Publisher<imu_pipeline::msg::ImuReading>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Time start_time_;
};
