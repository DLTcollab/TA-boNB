/*
 * Copyright (C) 2018-2019 BiiLabs Co., Ltd. and Contributors
 * All Rights Reserved.
 * This is free software; you can redistribute it and/or modify it under the
 * terms of the MIT license. A copy of the license can be found in the file
 * "LICENSE" at the root of this distribution.
 */

#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CBC_IV_SIZE 16
#define AES_KEY_SIZE 32
#define MAXLINE 1024
#define IMSI_LEN 15

int get_device_id(uint8_t *device_id);
int get_aes_key(uint8_t *key);
int ta_encrypt(const char *plaintext, int plaintext_len, char *ciphertext, int ciphertext_len, uint8_t iv[CBC_IV_SIZE],
               uint8_t key[AES_KEY_SIZE], uint8_t device_id[IMSI_LEN + 1]);
int ta_decrypt(const char *ciphertext, int ciphertext_len, char *plaintext, int plaintext_len, uint8_t iv[CBC_IV_SIZE],
               uint8_t key[AES_KEY_SIZE]);
int aes_encrypt(const char *plaintext, int plaintext_len, const unsigned char *key, unsigned int keybits,
                unsigned char iv[CBC_IV_SIZE], char *ciphertext, int ciphertext_len);
int aes_decrypt(const char *ciphertext, int ciphertext_len, const unsigned char *key, unsigned int keybits,
                unsigned char iv[CBC_IV_SIZE], char *plaintext, int plaintext_len);

#ifdef __cplusplus
}
#endif

#endif  // CRYPTO_UTILS_H
