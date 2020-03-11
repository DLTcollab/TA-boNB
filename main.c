#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "conn_http.h"
#include "crypto_utils.h"
#include "serializer.h"
#include "tryte_byte_conv.h"
#include "uart_utils.h"
#include "url.h"

#ifndef DEBUG
#define MSG "%s:%s"
#else
#define MSG "%s:THISISMSG9THISISMSG9THISISMSG"
#endif

#define SSL_SEED "nonce"
#define ADDRESS                                                                \
  "POWEREDBYTANGLEACCELERATOR999999999999999999999999999999999999999999999999" \
  "999999A"
#define ADDR_LEN 81
#define ADDR_LOG_PATH "ta-endpoint.log"

#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)

const char *HOST = STRINGIZE_VALUE_OF(TA_HOST);
const char *PORT = STRINGIZE_VALUE_OF(TA_PORT);
const char *API = STRINGIZE_VALUE_OF(TA_API);

int main(int argc, char *argv[]) {
  uint8_t addr[ADDR_LEN] = ADDRESS, next_addr[ADDR_LEN] = {0}, iv[16] = {0};
  char raw_msg[1000] = {0}, ciphertext[1024] = {0};
  char tryte_msg[1024] = {0}, msg[1024] = {0};
  uint32_t raw_msg_len = 1 + ADDR_LEN + 20, ciphertext_len = 0, msg_len = 0;

  srand(time(NULL));

#ifndef DEBUG
  int fd = uart_init();
  if (fd < 0) {
    printf("Error in initializing UART\n");
    return -1;
  }
#else
  if (write_address(ADDR_LOG_PATH, next_addr, ADDR_LEN) != 0) {
    fprintf(stderr, "log address failed\n");
    return -1;
  }
#endif

  char *response = NULL;
  time_t timer;
  char time_str[26];
  struct tm *tm_info;

#ifndef DEBUG
  fd_set rset;
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 500;
  while (true) {
    FD_ZERO(&rset);
    FD_SET(fd, &rset);
    select(fd + 1, &rset, NULL, NULL, &tv);

    if (FD_ISSET(fd, &rset)) {
#endif
      time(&timer);
      tm_info = localtime(&timer);
      strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);
      printf("%s\n", time_str);
      gen_rand_trytes(ADDR_LEN, next_addr);

#ifndef DEBUG
      response = uart_read(fd);
#else
  response = strdup("This is a test");
  printf("next_addr = %.*s \n", ADDR_LEN, next_addr);
  write_address(ADDR_LOG_PATH, next_addr, ADDR_LEN);
#endif
      // real transmitted data
#ifndef DEBUG
      snprintf(raw_msg, raw_msg_len, MSG, next_addr, response);
#else
  snprintf(raw_msg, raw_msg_len, MSG, next_addr);
#endif
      printf("Raw Message: %s\n", raw_msg);
      uint8_t private_key[AES_KEY_SIZE] = {0};
      uint8_t id[IMSI_LEN] = {0};
#ifndef DEBUG
      if (get_aes_key(private_key) != 0) {
        fprintf(stderr, "%s\n", "get aes key error");
        return -1;
      }
      // fetch Device_ID (IMSI, len <= 16)
      if (get_device_id(id) != 0) {
        fprintf(stderr, "%s\n", "get device id error");
        return -1;
      }
#else
  /* Setting data to produce predictable results during debugging */
  // init vector for aes
  const uint8_t iv_global[16] = {164, 3, 98, 193, 52, 162, 107, 252, 184, 42, 74, 225, 157, 26, 88, 72};
  // device id
  const char *device_id = "470010171566423";
  // private key
  const uint8_t key[32] = {82,  142, 184, 64,  74, 105, 126, 65,  154, 116, 14,  193, 208, 41,  8,  115,
                           158, 252, 228, 160, 79, 5,   167, 185, 13,  159, 135, 113, 49,  209, 58, 68};

  memcpy(id, device_id, 16);
  memcpy(private_key, key, 16);
  memcpy(iv, iv_global, 16);
#endif
      ciphertext_len = ta_encrypt(raw_msg, strlen(raw_msg), ciphertext, 1024, iv, private_key, id);
      if (ciphertext_len == 0) {
        fprintf(stderr, "%s\n", "ta_encrypt msg error");
        return -1;
      }
      serialize_msg(iv, ciphertext_len, ciphertext, msg, &msg_len);
      bytes_to_trytes((const unsigned char *)msg, msg_len, tryte_msg);

      // Init http session. verify: check the server CA cert.
      char msg_body[1024];
      gen_tryte_msg(tryte_msg, addr, msg_body);
      if (send_https_msg(HOST, PORT, API, msg_body, 1024, SSL_SEED) != HTTP_OK) {
        fprintf(stderr, "Response from ta server failed\n");
      }

      memcpy(addr, next_addr, ADDR_LEN);
      free(response);
      response = NULL;
      printf(
          "========================Finishing Sending "
          "Transaction========================\n\n");
#ifndef DEBUG
    }
    if (tcflush(fd, TCIOFLUSH) != 0) {
      perror("tcflush error");
    }
  }
#endif
  return 0;
}
