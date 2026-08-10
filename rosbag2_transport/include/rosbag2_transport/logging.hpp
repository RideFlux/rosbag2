// Copyright 2018 Bosch Software Innovations GmbH.
// Licensed under the Apache License, Version 2.0.

#ifndef ROSBAG2_TRANSPORT__LOGGING_HPP_
#define ROSBAG2_TRANSPORT__LOGGING_HPP_

#include <sstream>

#include "rcutils/logging_macros.h"

#define ROSBAG2_TRANSPORT_PACKAGE_NAME "rosbag2_transport"
#define ROSBAG2_TRANSPORT_LOG_INFO(...) \
  RCUTILS_LOG_INFO_NAMED(ROSBAG2_TRANSPORT_PACKAGE_NAME, __VA_ARGS__)
#define ROSBAG2_TRANSPORT_LOG_INFO_STREAM(args) do { \
    std::stringstream stream; stream << args; \
    RCUTILS_LOG_INFO_NAMED(ROSBAG2_TRANSPORT_PACKAGE_NAME, "%s", stream.str().c_str()); \
  } while (0)
#define ROSBAG2_TRANSPORT_LOG_ERROR(...) \
  RCUTILS_LOG_ERROR_NAMED(ROSBAG2_TRANSPORT_PACKAGE_NAME, __VA_ARGS__)
#define ROSBAG2_TRANSPORT_LOG_ERROR_STREAM(args) do { \
    std::stringstream stream; stream << args; \
    RCUTILS_LOG_ERROR_NAMED(ROSBAG2_TRANSPORT_PACKAGE_NAME, "%s", stream.str().c_str()); \
  } while (0)
#define ROSBAG2_TRANSPORT_LOG_WARN(...) \
  RCUTILS_LOG_WARN_NAMED(ROSBAG2_TRANSPORT_PACKAGE_NAME, __VA_ARGS__)
#define ROSBAG2_TRANSPORT_LOG_WARN_STREAM(args) do { \
    std::stringstream stream; stream << args; \
    RCUTILS_LOG_WARN_NAMED(ROSBAG2_TRANSPORT_PACKAGE_NAME, "%s", stream.str().c_str()); \
  } while (0)
#define ROSBAG2_TRANSPORT_LOG_DEBUG(...) \
  RCUTILS_LOG_DEBUG_NAMED(ROSBAG2_TRANSPORT_PACKAGE_NAME, __VA_ARGS__)
#define ROSBAG2_TRANSPORT_LOG_DEBUG_STREAM(args) do { \
    std::stringstream stream; stream << args; \
    RCUTILS_LOG_DEBUG_NAMED(ROSBAG2_TRANSPORT_PACKAGE_NAME, "%s", stream.str().c_str()); \
  } while (0)

#endif  // ROSBAG2_TRANSPORT__LOGGING_HPP_
