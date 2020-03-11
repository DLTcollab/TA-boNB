#include "crypto_utils.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mbedtls/aes.h"
#include "mbedtls/md.h"

#define MAX_TIMESTAMP_LEN 20

// The device ID we are used here is IMSI. We could use other physical ID in the
// future.
int get_device_id(uint8_t *device_id) {
  // TODO: replace cm command
  char result_buf[MAXLINE], *imsi;
  char cmd[] = "cm sim info";
  FILE *fp;

  fp = popen(cmd, "r");
  if (NULL == fp) {
    perror("popen open error");
    return -1;
  }

  while (fgets(result_buf, sizeof(result_buf), fp) != NULL) {
    if (strstr(result_buf, "IMSI")) {
      result_buf[strlen(result_buf) - 1] = '\0';
      imsi = strtok(result_buf + 5, " ");
    }
  }

  memcpy(device_id, imsi, IMSI_LEN);

  if (pclose(fp) == -1) {
    perror("close FILE pointer");
    return -1;
  }
  return 0;
}

// Get AES key with hashchain in legato originated app form.
int get_aes_key(uint8_t *key) {
  // TODO: replace cm command
  char hash_chain_res[MAXLINE];
  char cmd[] = "cm sim info";
  FILE *fp;

  fp = popen(cmd, "r");

  if (NULL == fp) {
    perror("popen open error");
    return -1;
  }

  if (fgets(hash_chain_res, sizeof(hash_chain_res), fp) != NULL) {
    hash_chain_res[strlen(hash_chain_res) - 2] = '\0';
  }

  memcpy(key, hash_chain_res, CBC_IV_SIZE);

  if (pclose(fp) == -1) {
    perror("close FILE pointer");
    return -1;
  }

  return 0;
}

int aes_encrypt(const char *plaintext, int plaintext_len, const unsigned char *key, unsigned int keybits,
                unsigned char iv[CBC_IV_SIZE], char *ciphertext, int ciphertext_len) {
  mbedtls_aes_context ctx;
  int status;
  unsigned char buf[CBC_IV_SIZE];
  int n = 0;
  char *err;
  /*
   * Initialise the encryption operation.
   */
  // Check ciphertext has enough space
  int new_len = plaintext_len + (CBC_IV_SIZE - plaintext_len % CBC_IV_SIZE);
  if (new_len > ciphertext_len) {
    err = "ciphertext has not enough space";
    goto exit;
  }
  mbedtls_aes_init(&ctx);
  memset(ciphertext, 0, ciphertext_len);
  /* set encryption key */
  if ((status = mbedtls_aes_setkey_enc(&ctx, key, keybits)) != 0) {
    err = "set aes key failed";
    goto exit;
  }

  /*
   * Encrypt plaintext
   */
  for (int i = 0; i < new_len; i += CBC_IV_SIZE) {
    memset(buf, 0, CBC_IV_SIZE);
    n = (new_len - i > CBC_IV_SIZE) ? CBC_IV_SIZE : (int)(new_len - i);
    memcpy(buf, plaintext + i, n);
    if ((status = mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, CBC_IV_SIZE, iv, buf, buf)) != 0) {
      err = "aes decrpyt failed";
      goto exit;
    }
    memcpy(ciphertext, buf, CBC_IV_SIZE);
    ciphertext += CBC_IV_SIZE;
  }

  /* Clean up */
  mbedtls_aes_free(&ctx);
  return new_len;
exit:
  fprintf(stderr, "%s\n", err);
  mbedtls_aes_free(&ctx);
  return -1;
}

int aes_decrypt(const char *ciphertext, int ciphertext_len, const unsigned char *key, unsigned int keybits,
                unsigned char iv[CBC_IV_SIZE], char *plaintext, int plaintext_len) {
  mbedtls_aes_context ctx;
  int status, n = 0;
  char *err;
  uint8_t buf[CBC_IV_SIZE];
  /* Create and initialise the context */
  mbedtls_aes_init(&ctx);
  memset(plaintext, 0, plaintext_len);

  /* set decryption key */
  if ((status = mbedtls_aes_setkey_dec(&ctx, key, keybits)) != EXIT_SUCCESS) {
    err = "set aes key failed";
    goto exit;
  }

  /*
   * Provide the message to be decrypted, and obtain the plaintext output.
   */

  for (int i = 0; i < ciphertext_len; i += CBC_IV_SIZE) {
    memset(buf, 0, CBC_IV_SIZE);
    n = (ciphertext_len - i > CBC_IV_SIZE) ? CBC_IV_SIZE : (int)(ciphertext_len - i);
    memcpy(buf, ciphertext + i, n);
    if ((status = mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_DECRYPT, CBC_IV_SIZE, iv, buf, buf)) != 0) {
      err = "aes decrpyt failed";
      goto exit;
    }
    memcpy(plaintext, buf, CBC_IV_SIZE);
    plaintext += CBC_IV_SIZE;
  }

  /* Clean up */
  mbedtls_aes_free(&ctx);
  return 0;
exit:
  fprintf(stderr, "%s\n", err);
  mbedtls_aes_free(&ctx);
  return -1;
}

int ta_encrypt(const char *plaintext, int plaintext_len, char *ciphertext, int ciphertext_len, uint8_t iv[CBC_IV_SIZE],
               uint8_t key[AES_KEY_SIZE], uint8_t device_id[IMSI_LEN + 1]) {
  int new_len = 0;
  char *err = NULL;
  uint8_t tmp[CBC_IV_SIZE];
  char nonce[IMSI_LEN + MAX_TIMESTAMP_LEN + 1 + 1] = {0};
  uint8_t digest[32];
  uint8_t buffer[MAXLINE];
  mbedtls_md_context_t sha_ctx;

  mbedtls_md_init(&sha_ctx);
  if (mbedtls_md_setup(&sha_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1) != 0) {
    err = "mbedtls_md_setup error";
    goto exit;
  }

  memset(tmp, 0, sizeof(tmp));
  memset(digest, 0, sizeof(digest));
  memset(buffer, 0, sizeof(buffer));

  // fetch timestamp
  uint64_t timestamp = time(NULL);
  // concatenate (Device_ID, timestamp)
  snprintf(nonce, IMSI_LEN + MAX_TIMESTAMP_LEN + 1, "%s-%" PRIu64, device_id, timestamp);
  // hash base data
#ifndef DEBUG
  mbedtls_md_starts(&sha_ctx);
  mbedtls_md_update(&sha_ctx, (unsigned char *)nonce, IMSI_LEN + MAX_TIMESTAMP_LEN);
  mbedtls_md_finish(&sha_ctx, digest);

  mbedtls_md_starts(&sha_ctx);
  mbedtls_md_update(&sha_ctx, digest, 32);
  mbedtls_md_finish(&sha_ctx, digest);

  for (int i = 0; i < CBC_IV_SIZE; ++i) {
    tmp[i] = digest[i] ^ digest[i + CBC_IV_SIZE];
  }
  memcpy(iv, tmp, CBC_IV_SIZE);
#else
  memcpy(tmp, iv, CBC_IV_SIZE);
#endif
  new_len = aes_encrypt(plaintext, plaintext_len, key, 256, tmp, ciphertext, ciphertext_len);
#ifdef DEBUG
  printf("aes encrypt passed\n");
#endif
exit:
  if (err) fprintf(stderr, "%s\n", err);
  mbedtls_md_free(&sha_ctx);
  return new_len;
}

int ta_decrypt(const char *ciphertext, int ciphertext_len, char *plaintext, int plaintext_len, uint8_t iv[CBC_IV_SIZE],
               uint8_t key[AES_KEY_SIZE]) {
  aes_decrypt(ciphertext, ciphertext_len, key, 256, iv, plaintext, plaintext_len);
  return 0;
}
