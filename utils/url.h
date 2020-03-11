/*
 * Copyright (C) 2019-2020 BiiLabs Co., Ltd. and Contributors
 * All Rights Reserved.
 * This is free software; you can redistribute it and/or modify it under the
 * terms of the MIT license. A copy of the license can be found in the file
 * "LICENSE" at the root of this distribution.
 */
#ifndef UTILS_URL_H
#define UTILS_URL_H

#include <stdint.h>
#include <stdio.h>

#define REQ_BODY                                                           \
  "{\"value\": 0, \"tag\": \"POWEREDBYTANGLEACCELERATOR9\", \"message\": " \
  "\"%s\", \"address\":\"%s\"}\r\n\r\n"

#define RET_SUCCESS 0
#define RET_FAILED 0xff

/**
 * @brief generate trytes from random
 *
 * @param len length of output
 * @param out output array
 */
void gen_rand_trytes(uint16_t len, uint8_t *out);

/**
 * @brief write address into log file
 *
 * @param file file to log
 * @param next_addr address to log
 * @param addr_len length of next_addr array
 *
 * @return log success 0 else -1
 * @see gen_trytes()
 */
int write_address(const char *file, uint8_t *next_addr, size_t addr_len);

/**
 * @brief create message to send to tangle accerlator
 *
 * @param tryte_msg byte message which has convert to tryte
 * @param addr IOTA next address
 * @param req_body[1024] message array
 */
void gen_tryte_msg(const char *tryte_msg, const uint8_t *addr, char req_body[1024]);

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
int send_https_msg(char const *host, char const *port, char const *api, const char *msg, const int msg_len,
                   const char *ssl_seed);
#endif /* end of include guard: UTILS_URL_H */
