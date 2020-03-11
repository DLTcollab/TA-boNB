/*
 * Copyright (C) 2019-2020 BiiLabs Co., Ltd. and Contributors
 * All Rights Reserved.
 * This is free software; you can redistribute it and/or modify it under the
 * terms of the MIT license. A copy of the license can be found in the file
 * "LICENSE" at the root of this distribution.
 */

#ifndef HAL_DEVICE_H
#define HAL_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DECLARE_DEVICE(name)                                  \
  void init_##name##_init(void) __attribute__((constructor)); \
  void init_##name##_init(void) { name##_device_type.op->init(); }

typedef struct device_type device_t;
enum { DEVICE_OK, DEVICE_ERROR };

struct device_operations {
  int (*init)(void);            /**< init for device */
  void (*fini)(void);           /**< destructor for device  */
  int (*get_key)(uint8_t *);    /**< get device private key */
  int (*get_device_id)(char *); /**< get device id          */
};

struct logging_operations {
  void (*write_log)(char *path, char *msg, size_t msg_len);
};

struct uart_operations {
  int (*init)(const char *device);
  void (*write)(const int fd, const char *cmd);
  char *(*read)(const int fd);
  void (*clean)(const int fd);
};

struct device_type {
  const char *name;
  const struct device_operations *op;
  const struct uart_operations *uart;
  const struct logging_operations *logger;
  device_t *next;
};

device_t *get_device(const char *);
int register_device(struct device_type *dv);
int unregister_device(struct device_type *dv);

#ifdef __cplusplus
}
#endif

#endif  // HAL_DEVICE_H
