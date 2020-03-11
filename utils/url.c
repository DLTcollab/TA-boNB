#include "url.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conn_http.h"
#include "http_parser.h"

static char addr_log_template[] = "\n%s\n";

void gen_rand_trytes(uint16_t len, uint8_t *out) {
  const char tryte_alphabet[] = "9ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  uint8_t rand_index;
  for (int i = 0; i < len; i++) {
    rand_index = rand() % 27;
    out[i] = tryte_alphabet[rand_index];
  }
}

void gen_tryte_msg(const char *tryte_msg, const uint8_t *addr, char req_body[1024]) {
  memset(req_body, '0', sizeof(char) * 1024);
  snprintf(req_body, 1024, REQ_BODY, tryte_msg, addr);

#ifdef DEBUG
  printf("req packet = \n%s", req_body);
#endif
}

int send_https_msg(char const *host, char const *port, char const *api, const char *msg, const int msg_len,
                   const char *ssl_seed) {
  char res[4096] = {0};
  char *req = NULL;
  int ret = 0;

  set_post_request(api, host, atoi(port), msg, &req);
  http_parser_settings settings;
  settings.on_body = parser_body_callback;
  http_parser *parser = malloc(sizeof(http_parser));

  connect_info_t info = {.https = true};
  http_open(&info, ssl_seed, host, port);
  http_send_request(&info, req);
  http_read_response(&info, res, sizeof(res) / sizeof(char));
  http_close(&info);
  http_parser_init(parser, HTTP_RESPONSE);
  http_parser_execute(parser, &settings, res, strlen(res));
#ifdef DEBUG
  printf("HTTP Response: %s\n", http_res_body);
#endif
  free(http_res_body);
  http_res_body = NULL;

  ret = parser->status_code;
  free(parser);
  free(req);
  return ret;
}

int write_address(const char *path, uint8_t *next_addr, size_t addr_len) {
  FILE *fp;
  char addr_log[addr_len + 3];
  memset(addr_log, '0', addr_len + 3);

  fp = fopen(path, "a");
  if (!fp) {
    perror("log address failed:");
    fclose(fp);
    return RET_FAILED;
  }

  snprintf(addr_log, 83, addr_log_template, next_addr);
  fputs(addr_log, fp);
  fclose(fp);
  return RET_SUCCESS;
}
