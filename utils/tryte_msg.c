#include "tryte_msg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REQ_BODY                                                           \
  "{\"value\": 0, \"tag\": \"POWEREDBYTANGLEACCELERATOR9\", \"message\": " \
  "\"%s\", \"address\":\"%s\"}\r\n\r\n"

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
