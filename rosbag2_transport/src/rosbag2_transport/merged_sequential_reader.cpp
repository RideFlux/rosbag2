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

#include "rosbag2_transport/merged_sequential_reader.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include "rosbag2_transport/reader_writer_factory.hpp"

namespace rosbag2_transport
{

MergedSequentialReader::MergedSequentialReader(
  std::vector<rosbag2_storage::StorageOptions> storage_options)
: storage_options_(std::move(storage_options))
{}

void MergedSequentialReader::open(
  const rosbag2_storage::StorageOptions &,
  const rosbag2_cpp::ConverterOptions & converter_options)
{
  if (storage_options_.empty()) {
    throw std::runtime_error("No input bags were provided.");
  }

  reset();
  metadata_.storage_identifier = "merged";
  std::unordered_map<std::string, size_t> topic_indexes;
  bool has_time_bounds = false;
  decltype(metadata_.starting_time) latest_end_time{};

  for (const auto & storage_options : storage_options_) {
    auto reader = ReaderWriterFactory::make_reader(storage_options);
    reader->open(storage_options, converter_options);
    if (!topics_filter_.topics.empty()) {
      reader->set_filter(topics_filter_);
    }

    const auto & bag_metadata = reader->get_metadata();
    metadata_.version = std::max(metadata_.version, bag_metadata.version);
    metadata_.bag_size += bag_metadata.bag_size;
    metadata_.message_count += bag_metadata.message_count;
    metadata_.relative_file_paths.insert(
      metadata_.relative_file_paths.end(),
      bag_metadata.relative_file_paths.begin(), bag_metadata.relative_file_paths.end());

    const auto bag_end_time = bag_metadata.starting_time + bag_metadata.duration;
    if (!has_time_bounds || bag_metadata.starting_time < metadata_.starting_time) {
      metadata_.starting_time = bag_metadata.starting_time;
    }
    if (!has_time_bounds || latest_end_time < bag_end_time) {
      latest_end_time = bag_end_time;
    }
    has_time_bounds = true;

    for (const auto & topic_information : bag_metadata.topics_with_message_count) {
      const auto & topic = topic_information.topic_metadata;
      const auto existing = topic_indexes.find(topic.name);
      if (existing == topic_indexes.end()) {
        topic_indexes.emplace(topic.name, metadata_.topics_with_message_count.size());
        metadata_.topics_with_message_count.push_back(topic_information);
        topics_metadata_.push_back(topic);
      } else {
        auto & merged_topic = metadata_.topics_with_message_count[existing->second];
        if (merged_topic.topic_metadata.type != topic.type ||
          merged_topic.topic_metadata.serialization_format != topic.serialization_format)
        {
          throw std::runtime_error(
                  "Input bags contain topic '" + topic.name +
                  "' with conflicting type or serialization format.");
        }
        merged_topic.message_count += topic_information.message_count;
      }
    }

    readers_.push_back({std::move(reader), nullptr});
  }

  if (has_time_bounds) {
    metadata_.duration = latest_end_time - metadata_.starting_time;
  }
  is_open_ = true;
}

void MergedSequentialReader::close()
{
  for (auto & reader_state : readers_) {
    reader_state.reader->close();
  }
  reset();
}

bool MergedSequentialReader::has_next()
{
  assert_open();
  for (size_t i = 0; i < readers_.size(); ++i) {
    if (ensure_next_message(i)) {
      return true;
    }
  }
  return false;
}

rosbag2_storage::SerializedBagMessageSharedPtr MergedSequentialReader::read_next()
{
  assert_open();
  size_t selected_reader = readers_.size();
  for (size_t i = 0; i < readers_.size(); ++i) {
    if (!ensure_next_message(i)) {
      continue;
    }
    if (selected_reader == readers_.size() ||
      readers_[i].next_message->time_stamp < readers_[selected_reader].next_message->time_stamp)
    {
      selected_reader = i;
    }
  }
  if (selected_reader == readers_.size()) {
    throw std::runtime_error("No more messages are available.");
  }
  auto message = readers_[selected_reader].next_message;
  readers_[selected_reader].next_message.reset();
  return message;
}

const rosbag2_storage::BagMetadata & MergedSequentialReader::get_metadata() const
{
  assert_open();
  return metadata_;
}

std::vector<rosbag2_storage::TopicMetadata>
MergedSequentialReader::get_all_topics_and_types() const
{
  assert_open();
  return topics_metadata_;
}

void MergedSequentialReader::set_filter(
  const rosbag2_storage::StorageFilter & storage_filter)
{
  assert_open();
  topics_filter_ = storage_filter;
  for (auto & reader_state : readers_) {
    reader_state.reader->set_filter(topics_filter_);
    reader_state.next_message.reset();
  }
}

void MergedSequentialReader::reset_filter()
{
  assert_open();
  topics_filter_ = rosbag2_storage::StorageFilter{};
  for (auto & reader_state : readers_) {
    reader_state.reader->reset_filter();
    reader_state.next_message.reset();
  }
}

void MergedSequentialReader::seek(const rcutils_time_point_value_t & timestamp)
{
  assert_open();
  for (auto & reader_state : readers_) {
    reader_state.reader->seek(timestamp);
    reader_state.next_message.reset();
  }
}

void MergedSequentialReader::add_event_callbacks(
  const rosbag2_cpp::bag_events::ReaderEventCallbacks & callbacks)
{
  assert_open();
  for (auto & reader_state : readers_) {
    auto reader_callbacks = callbacks;
    reader_state.reader->add_event_callbacks(reader_callbacks);
  }
}

bool MergedSequentialReader::ensure_next_message(size_t reader_index)
{
  auto & reader_state = readers_[reader_index];
  if (reader_state.next_message) {
    return true;
  }
  if (!reader_state.reader->has_next()) {
    return false;
  }
  reader_state.next_message = reader_state.reader->read_next();
  return true;
}

void MergedSequentialReader::assert_open() const
{
  if (!is_open_) {
    throw std::runtime_error("Bag is not open. Call open() before reading.");
  }
}

void MergedSequentialReader::reset()
{
  readers_.clear();
  metadata_ = rosbag2_storage::BagMetadata{};
  topics_metadata_.clear();
  is_open_ = false;
}

}  // namespace rosbag2_transport
