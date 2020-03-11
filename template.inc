#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "conn_http.h"
#include "crypto_utils.h"
#include "device.h"
#include "logger.h"
#include "serializer.h"
#include "tryte_byte_conv.h"
#include "tryte_msg.h"
#include "url.h"

#define SSL_SEED "nonce"
#define ADDRESS                                                                \
  "POWEREDBYTANGLEACCELERATOR999999999999999999999999999999999999999999999999" \
  "999999A"
#define ADDR_LEN 81

int runner(const char* host, const char* port, const char* api)
{
    uint8_t addr[ADDR_LEN] = ADDRESS, next_addr[ADDR_LEN] = {0}, iv[16] = {0};
    char raw_msg[1000] = {0}, ciphertext[1024] = {0};
    char tryte_msg[1024] = {0}, msg[1024] = {0};
    uint32_t raw_msg_len = 1 + ADDR_LEN + 20, msg_len = 0;
    srand(time(NULL));

    device_t *dv = get_device("{% device_name %}");
    int fd = dv->uart->init("{% uart_port %}");
    if (fd < 0) {
      LOG_ERROR("Error in initializing UART\n");
      return -1;
    }

    char* log_msg = malloc(sizeof(char)*ADDR_LEN + 3);
    memset(log_msg, '0', ADDR_LEN + 3);
    snprintf(log_msg, strlen(next_addr), "\n%s", next_addr);
    dv->logger->write_log("{% log_path %}", next_addr, strlen(next_addr));
    free(log_msg);

    char *response = NULL;
    time_t timer;
    char time_str[26];
    struct tm *tm_info;

    fd_set rset;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 500;
    while (true) {
      FD_ZERO(&rset);
      FD_SET(fd, &rset);
      select(fd + 1, &rset, NULL, NULL, &tv);

      if (FD_ISSET(fd, &rset)) {
        time(&timer);
        tm_info = localtime(&timer);
        strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);
        LOG_DEBUG("%s\n", time_str);
        gen_rand_trytes(ADDR_LEN, next_addr);

        response = dv->uart->read(fd);
        snprintf(raw_msg, raw_msg_len, "%s:%s", next_addr, response);

        LOG_DEBUG("Raw Message: %s\n", raw_msg);
        uint8_t private_key[AES_BLOCK_SIZE] = {0};
        uint8_t id[IMSI_LEN] = {0};

        if (dv->op->get_key(private_key) != 0) {
          LOG_ERROR("%s\n", "get aes key error");
          return -1;
        }

        // fetch Device_ID (IMSI, len <= 16)
        if (dv->op->get_device_id(id) != 0) {
          LOG_ERROR("%s\n", "get device id error");
          return -1;
        }
        int ciphertext_len = encrypt(raw_msg, strlen(raw_msg), ciphertext, 1024, iv, private_key, id);
        if (ciphertext_len == 0) {
          LOG_ERROR("encrypt msg error");
          return -1;
        }
        serialize_msg(iv, ciphertext_len, ciphertext, msg, &msg_len);
        bytes_to_trytes((const unsigned char *)msg, msg_len, tryte_msg);

        // Init http session. verify: check the server CA cert.
        char msg_body[1024];
        gen_tryte_msg(tryte_msg, addr, msg_body);
        if (send_https_msg(host, port, api, msg_body, 1024, SSL_SEED) != HTTP_OK) {
          LOG_ERROR("Response from ta server failed");
        }

        memcpy(addr, next_addr, ADDR_LEN);
        free(response);
        response = NULL;
        LOG_INFO("Finish transaction");
      }
      dv->uart->clean(fd);
    }
}
