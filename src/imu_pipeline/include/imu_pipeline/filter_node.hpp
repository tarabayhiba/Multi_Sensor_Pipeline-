#pragma once

#include <deque>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "imu_pipeline/msg/imu_reading.hpp"


class FilterNode : public rclcpp::Node
{
public:
  FilterNode() : Node("filter_node")
  {
    this->declare_parameter<int>("window_size", 10);
    window_size_ = this->get_parameter("window_size").as_int();
    if (window_size_ < 1) {
      window_size_ = 1;
    }

    rclcpp::QoS qos(rclcpp::KeepLast(20));
    qos.reliable();

    publisher_ = this->create_publisher<imu_pipeline::msg::ImuReading>("imu_filtered", qos);
    subscription_ = this->create_subscription<imu_pipeline::msg::ImuReading>(
      "imu_raw", qos,
      std::bind(&FilterNode::raw_callback, this, std::placeholders::_1));

    RCLCPP_INFO(this->get_logger(), "filter_node window_size=%ld", window_size_);
  }

private:
  double push_and_average(std::deque<double> & buf, double & sum, double value)
  {
    buf.push_back(value);
    sum += value;
    if (static_cast<int64_t>(buf.size()) > window_size_) {
      sum -= buf.front();
      buf.pop_front();
    }
    return sum / static_cast<double>(buf.size());
  }

  void raw_callback(const imu_pipeline::msg::ImuReading::SharedPtr msg)
  {
    auto out = imu_pipeline::msg::ImuReading();

    out.accel_x = push_and_average(buf_accel_x_, sum_accel_x_, msg->accel_x);
    out.accel_y = push_and_average(buf_accel_y_, sum_accel_y_, msg->accel_y);
    out.accel_z = push_and_average(buf_accel_z_, sum_accel_z_, msg->accel_z);
    out.gyro_x = push_and_average(buf_gyro_x_, sum_gyro_x_, msg->gyro_x);
    out.gyro_y = push_and_average(buf_gyro_y_, sum_gyro_y_, msg->gyro_y);
    out.gyro_z = push_and_average(buf_gyro_z_, sum_gyro_z_, msg->gyro_z);
    out.stamp = msg->stamp;

    publisher_->publish(out);
  }

  int64_t window_size_;

  std::deque<double> buf_accel_x_, buf_accel_y_, buf_accel_z_;
  std::deque<double> buf_gyro_x_, buf_gyro_y_, buf_gyro_z_;
  double sum_accel_x_ = 0.0, sum_accel_y_ = 0.0, sum_accel_z_ = 0.0;
  double sum_gyro_x_ = 0.0, sum_gyro_y_ = 0.0, sum_gyro_z_ = 0.0;

  rclcpp::Publisher<imu_pipeline::msg::ImuReading>::SharedPtr publisher_;
  rclcpp::Subscription<imu_pipeline::msg::ImuReading>::SharedPtr subscription_;
};
