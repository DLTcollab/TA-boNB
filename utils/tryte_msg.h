/*
 * Copyright (C) 2019-2020 BiiLabs Co., Ltd. and Contributors
 * All Rights Reserved.
 * This is free software; you can redistribute it and/or modify it under the
 * terms of the MIT license. A copy of the license can be found in the file
 * "LICENSE" at the root of this distribution.
 */

#ifndef TRYTE_MSG_H
#define TRYTE_MSG_H

#include <stdint.h>

/**
 * @brief generate trytes from random
 *
 * @param len length of output
 * @param out output array
 */
void gen_rand_trytes(uint16_t len, uint8_t *out);

/**
 * @brief create message to send to tangle accerlator
 *
 * @param tryte_msg byte message which has convert to tryte
 * @param addr IOTA next address
 * @param req_body[1024] message array
 */
void gen_tryte_msg(const char *tryte_msg, const uint8_t *addr, char req_body[1024]);

#endif  // TRYTE_MSG_H
