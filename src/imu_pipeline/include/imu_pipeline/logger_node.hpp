#pragma once

#include <chrono>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "imu_pipeline/msg/imu_reading.hpp"

class LoggerNode : public rclcpp::Node
{
public:
  LoggerNode() : Node("logger_node")
  {
    this->declare_parameter<double>("log_rate_hz", 1.0);
    const double log_rate_hz = this->get_parameter("log_rate_hz").as_double();

    rclcpp::QoS qos(rclcpp::KeepLast(20));
    qos.reliable();

    raw_sub_ = this->create_subscription<imu_pipeline::msg::ImuReading>(
      "imu_raw", qos,
      [this](const imu_pipeline::msg::ImuReading::SharedPtr msg) {latest_raw_ = msg;});

    filtered_sub_ = this->create_subscription<imu_pipeline::msg::ImuReading>(
      "imu_filtered", qos,
      [this](const imu_pipeline::msg::ImuReading::SharedPtr msg) {latest_filtered_ = msg;});

    const auto period = std::chrono::duration<double>(1.0 / log_rate_hz);
    timer_ = this->create_wall_timer(
      std::chrono::duration_cast<std::chrono::nanoseconds>(period),
      std::bind(&LoggerNode::log_status, this));
  }

private:
  void log_status()
  {
    if (!latest_raw_ || !latest_filtered_) {
      RCLCPP_INFO(this->get_logger(), "waiting for data...");
      return;
    }

    const double raw = latest_raw_->accel_x;
    const double filtered = latest_filtered_->accel_x;
    RCLCPP_INFO(
      this->get_logger(),
      "raw accel_x=%.4f  filtered accel_x=%.4f  diff=%.4f",
      raw, filtered, raw - filtered);
  }

  rclcpp::Subscription<imu_pipeline::msg::ImuReading>::SharedPtr raw_sub_;
  rclcpp::Subscription<imu_pipeline::msg::ImuReading>::SharedPtr filtered_sub_;
  rclcpp::TimerBase::SharedPtr timer_;
  imu_pipeline::msg::ImuReading::SharedPtr latest_raw_;
  imu_pipeline::msg::ImuReading::SharedPtr latest_filtered_;
};
