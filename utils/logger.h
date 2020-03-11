/*
 * Copyright (C) 2019-2020 BiiLabs Co., Ltd. and Contributors
 * All Rights Reserved.
 * This is free software; you can redistribute it and/or modify it under the
 * terms of the MIT license. A copy of the license can be found in the file
 * "LICENSE" at the root of this distribution.
 */

#ifndef UTIL_LOGGER_H
#define UTIL_LOGGER_H

enum { LV_DEBUG = 10, LV_INFO = 20, LV_WARN = 30, LV_ERROR = 40, LV_CRIT = 50, LV_EMERG = 60 };

int get_log_level(void);
void set_log_level(int level);

#define LOG_DEBUG(fmt, ...)                                                                                         \
  do {                                                                                                              \
    if (get_log_level() <= LV_DEBUG) fprintf(stderr, "[%s:%d]:DEBUG:" fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
  } while (0)
#define LOG_INFO(fmt, ...)                                                                                        \
  do {                                                                                                            \
    if (get_log_level() <= LV_INFO) fprintf(stderr, "[%s:%d]:INFO:" fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
  } while (0)
#define LOG_WARN(fmt, ...)                                                                                        \
  do {                                                                                                            \
    if (get_log_level() <= LV_WARN) fprintf(stderr, "[%s:%d]:WARN:" fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
  } while (0)
#define LOG_ERROR(fmt, ...)                                                                                         \
  do {                                                                                                              \
    if (get_log_level() <= LV_ERROR) fprintf(stderr, "[%s:%d]:ERROR:" fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
  } while (0)
#define LOG_CRIT(fmt, ...)                                                                                        \
  do {                                                                                                            \
    if (get_log_level() <= LV_CRIT) fprintf(stderr, "[%s:%d]:CRIT:" fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
  } while (0)
#define LOG_EMERG(fmt, ...)                                                                                         \
  do {                                                                                                              \
    if (get_log_level() <= LV_EMERG) fprintf(stderr, "[%s:%d]:EMERG:" fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
  } while (0)

#endif  // UTIL_LOGGER_H
