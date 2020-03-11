/*
 * Copyright (C) 2019-2020 BiiLabs Co., Ltd. and Contributors
 * All Rights Reserved.
 * This is free software; you can redistribute it and/or modify it under the
 * terms of the MIT license. A copy of the license can be found in the file
 * "LICENSE" at the root of this distribution.
 */
#ifndef UTILS_URL_H
#define UTILS_URL_H

#include "defined_error.h"
/**
 * @brief send message vi http protocal
 *
 * @param host http host
 * @param port http port
 * @param api http api
 * @param msg message to send
 * @param msg_len length of message array
 * @param ssl_seed seed for ssl connection
 *
 * @return http status
 * @see http_retcode_t
 */
retcode_t send_https_msg(char const *host, char const *port, char const *api, const char *msg, const int msg_len,
                         const char *ssl_seed);
#endif  // UTILS_URL_H
