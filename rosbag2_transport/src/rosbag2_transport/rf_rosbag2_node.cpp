// Copyright 2026 RideFlux Inc.
// Licensed under the Apache License, Version 2.0.

#include "rosbag2_transport/rf_rosbag2_node.hpp"

#include <array>
#include <utility>

#include "rclcpp/serialization.hpp"

namespace rosbag2_transport
{

RfRosbag2Node::RfRosbag2Node(const std::string & node_name)
: rf_rclcpp::Node(node_name) {}

RfRosbag2Node::RfRosbag2Node(
  const std::string & node_name, const rclcpp::NodeOptions & options)
: rf_rclcpp::Node(node_name, options) {}

RfRosbag2Node::RfRosbag2Node(
  const std::string & node_name,
  const std::string & namespace_,
  const rclcpp::NodeOptions & options)
: rf_rclcpp::Node(node_name, namespace_, options) {}

GenericSubscription::SharedPtr RfRosbag2Node::createGenericSubscription(
  const std::string & topic,
  const std::string & type,
  const rclcpp::QoS & qos,
  std::function<void(std::shared_ptr<rclcpp::SerializedMessage>)> callback)
{
#ifdef RF_ROS_HIDE_TOPIC_TYPE
  auto wrapped_callback =
    [callback = std::move(callback)](std::shared_ptr<rclcpp::SerializedMessage> message) {
      rf_rclcpp::UInt8ArrayStampedMsg hidden_message;
      rclcpp::Serialization<rf_rclcpp::UInt8ArrayStampedMsg> serializer;
      serializer.deserialize_message(message.get(), &hidden_message);

#ifdef RF_ROS_ENCRYPT_TOPIC_DATA
      std::array<uint8_t, 16> key{};
      rf_rclcpp::UInt8ArrayStampedMsg encapsulated_message;
      encapsulated_message.header = hidden_message.header;
      encapsulated_message.data =
        rf_rclcpp::details::decrypt(key, key, hidden_message.data);
#else
      auto & encapsulated_message = hidden_message;
#endif
      auto decoded_message = std::make_shared<rclcpp::SerializedMessage>();
      rf_rclcpp::M_string header;
      if (rf_rclcpp::details::decapsulate_without_deserialization(
          encapsulated_message, *decoded_message, header))
      {
        callback(decoded_message);
      }
    };
#else
  auto wrapped_callback = std::move(callback);
#endif

  return this->create_generic_subscription(topic, type, qos, std::move(wrapped_callback));
}

}  // namespace rosbag2_transport
