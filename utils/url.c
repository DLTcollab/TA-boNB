#include "url.h"
#include <stdlib.h>
#include <string.h>
#include "conn_http.h"
#include "http_parser.h"

retcode_t send_https_msg(char const *host, char const *port, char const *api, const char *msg, const int msg_len,
                         const char *ssl_seed) {
  char res[4096] = {0};
  char *req = NULL;
  retcode_t ret = 0;

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
