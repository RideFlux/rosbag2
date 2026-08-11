// Copyright 2026 RideFlux Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ROSBAG2_TRANSPORT__MERGED_SEQUENTIAL_READER_HPP_
#define ROSBAG2_TRANSPORT__MERGED_SEQUENTIAL_READER_HPP_

#include <cstddef>
#include <memory>
#include <vector>

#include "rosbag2_cpp/bag_events.hpp"
#include "rosbag2_cpp/reader.hpp"
#include "rosbag2_cpp/reader_interfaces/base_reader_interface.hpp"
#include "rosbag2_storage/bag_metadata.hpp"
#include "rosbag2_storage/serialized_bag_message.hpp"
#include "rosbag2_storage/storage_filter.hpp"
#include "rosbag2_storage/storage_options.hpp"
#include "rosbag2_storage/topic_metadata.hpp"
#include "rosbag2_transport/visibility_control.hpp"

namespace rosbag2_transport
{

class ROSBAG2_TRANSPORT_PUBLIC MergedSequentialReader
  : public rosbag2_cpp::reader_interfaces::BaseReaderInterface
{
public:
  explicit MergedSequentialReader(
    std::vector<rosbag2_storage::StorageOptions> storage_options);

  void open(
    const rosbag2_storage::StorageOptions & storage_options,
    const rosbag2_cpp::ConverterOptions & converter_options) override;
  void close() override;
  bool has_next() override;
  std::shared_ptr<rosbag2_storage::SerializedBagMessage> read_next() override;
  const rosbag2_storage::BagMetadata & get_metadata() const override;
  std::vector<rosbag2_storage::TopicMetadata> get_all_topics_and_types() const override;
  void set_filter(const rosbag2_storage::StorageFilter & storage_filter) override;
  void reset_filter() override;
  void seek(const rcutils_time_point_value_t & timestamp) override;
  void add_event_callbacks(
    const rosbag2_cpp::bag_events::ReaderEventCallbacks & callbacks) override;

private:
  struct ReaderState
  {
    std::unique_ptr<rosbag2_cpp::Reader> reader;
    rosbag2_storage::SerializedBagMessageSharedPtr next_message;
  };

  bool ensure_next_message(size_t reader_index);
  void assert_open() const;
  void reset();

  std::vector<rosbag2_storage::StorageOptions> storage_options_;
  std::vector<ReaderState> readers_;
  rosbag2_storage::StorageFilter topics_filter_;
  rosbag2_storage::BagMetadata metadata_;
  std::vector<rosbag2_storage::TopicMetadata> topics_metadata_;
  bool is_open_{false};
};

}  // namespace rosbag2_transport

#endif  // ROSBAG2_TRANSPORT__MERGED_SEQUENTIAL_READER_HPP_
