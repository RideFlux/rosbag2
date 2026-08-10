// Copyright 2026 RideFlux Inc.
// Licensed under the Apache License, Version 2.0.

#ifndef ROSBAG2_TRANSPORT__ROSBAG2_NODE_HPP_
#define ROSBAG2_TRANSPORT__ROSBAG2_NODE_HPP_

#include <string>

#include "rclcpp/node.hpp"
#include "rclcpp/node_options.hpp"

#include "rosbag2_transport/generic_publisher.hpp"
#include "rosbag2_transport/generic_subscription.hpp"

namespace rosbag2_transport
{

class Rosbag2Node : public rclcpp::Node
{
public:
  explicit Rosbag2Node(const std::string & node_name)
  : rclcpp::Node(node_name) {}

  Rosbag2Node(const std::string & node_name, const rclcpp::NodeOptions & options)
  : rclcpp::Node(node_name, options) {}

  ~Rosbag2Node() override = default;
};

}  // namespace rosbag2_transport

#endif  // ROSBAG2_TRANSPORT__ROSBAG2_NODE_HPP_
