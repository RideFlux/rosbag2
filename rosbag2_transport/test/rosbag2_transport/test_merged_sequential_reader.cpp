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

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "rcpputils/filesystem_helper.hpp"
#include "rosbag2_cpp/writer.hpp"
#include "rosbag2_storage/serialized_bag_message.hpp"
#include "rosbag2_storage/storage_options.hpp"
#include "rosbag2_storage/topic_metadata.hpp"
#include "rosbag2_test_common/memory_management.hpp"
#include "rosbag2_test_common/temporary_directory_fixture.hpp"
#include "rosbag2_transport/merged_sequential_reader.hpp"
#include "test_msgs/msg/basic_types.hpp"

class MergedSequentialReaderTest : public rosbag2_test_common::TemporaryDirectoryFixture
{
protected:
  void write_bag(const std::string & name, const std::vector<int64_t> & timestamps)
  {
    rosbag2_storage::StorageOptions options;
    options.uri = (rcpputils::fs::path(temporary_dir_path_) / name).string();
    options.storage_id = "sqlite3";

    rosbag2_cpp::Writer writer;
    writer.open(options, {"", ""});
    writer.create_topic({"/topic", "test_msgs/msg/BasicTypes", "cdr", ""});
    for (const auto timestamp : timestamps) {
      auto message = std::make_shared<rosbag2_storage::SerializedBagMessage>();
      auto test_message = std::make_shared<test_msgs::msg::BasicTypes>();
      message->topic_name = "/topic";
      message->time_stamp = timestamp;
      message->serialized_data = memory_management_.serialize_message(test_message);
      writer.write(message);
    }
    writer.close();
    storage_options_.push_back(options);
  }

  rosbag2_test_common::MemoryManagement memory_management_;
  std::vector<rosbag2_storage::StorageOptions> storage_options_;
};

TEST_F(MergedSequentialReaderTest, reads_messages_from_all_bags_in_timestamp_order)
{
  write_bag("first", {10, 30});
  write_bag("second", {20, 40});

  rosbag2_transport::MergedSequentialReader reader(storage_options_);
  reader.open(storage_options_.front(), {"", ""});

  std::vector<int64_t> timestamps;
  while (reader.has_next()) {
    timestamps.push_back(reader.read_next()->time_stamp);
  }

  EXPECT_EQ((std::vector<int64_t>{10, 20, 30, 40}), timestamps);
}
