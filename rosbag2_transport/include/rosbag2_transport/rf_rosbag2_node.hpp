// Copyright 2026 RideFlux Inc.
// Licensed under the Apache License, Version 2.0.

#ifndef ROSBAG2_TRANSPORT__RF_ROSBAG2_NODE_HPP_
#define ROSBAG2_TRANSPORT__RF_ROSBAG2_NODE_HPP_

#include <functional>
#include <memory>
#include <string>

#include "rclcpp/node_options.hpp"
#include "rclcpp/qos.hpp"
#include "rclcpp/serialized_message.hpp"
#include "rf_rclcpp/node.hpp"

#include "rosbag2_transport/generic_subscription.hpp"

namespace rosbag2_transport
{

class RfRosbag2Node : public rf_rclcpp::Node
{
public:
  explicit RfRosbag2Node(const std::string & node_name);
  RfRosbag2Node(const std::string & node_name, const rclcpp::NodeOptions & options);
  RfRosbag2Node(
    const std::string & node_name,
    const std::string & namespace_,
    const rclcpp::NodeOptions & options);
  ~RfRosbag2Node() override = default;

  GenericSubscription::SharedPtr createGenericSubscription(
    const std::string & topic,
    const std::string & type,
    const rclcpp::QoS & qos,
    std::function<void(std::shared_ptr<rclcpp::SerializedMessage>)> callback);
};

}  // namespace rosbag2_transport

#endif  // ROSBAG2_TRANSPORT__RF_ROSBAG2_NODE_HPP_
