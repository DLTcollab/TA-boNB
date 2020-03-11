ifeq ($(ARM), y)
CROSS = arm-linux-gnueabi-
CC = $(CROSS)gcc
CXX = $(CROSS)g++
AR = $(CROSS)ar
RANLIB = $(CROSS)ranlib
LD = $(CROSS)ld
STRIP = $(CROSS)strip
export CC CXX AR RANLIB
endif

ROOT_DIR = $(CURDIR)
THIRD_PARTY_PATH = $(ROOT_DIR)/third_party
MBEDTLS_PATH = $(THIRD_PARTY_PATH)/mbedtls
HTTP_PARSER_PATH = $(THIRD_PARTY_PATH)/http-parser
TEST_PATH = $(ROOT_DIR)/tests
UTILS_PATH = $(ROOT_DIR)/utils
CONNECTIVITY_PATH = $(ROOT_DIR)/connectivity
HAL_PATH = $(ROOT_DIR)/hal
DEVICES_PATH = $(ROOT_DIR)/devices
export THIRD_PARTY_PATH ROOT_DIR UTILS_PATH MBEDTLS_PATH HAL_PATH DEVICES_PATH HTTP_PARSER_PATH

ifeq ($(DEBUG), n)
CFLAGS = -Wall -Werror -fPIC -DHAVE_CONFIG_H -D_U_="__attribute__((unused))" -O2
else
CFLAGS = -Wall -fPIC -DHAVE_CONFIG_H -D_U_="__attribute__((unused))" -g3 -DDEBUG
endif
export CFLAGS

INCLUDES = -I$(THIRD_PARTY_PATH)/http-parser -I$(MBEDTLS_PATH)/include -I$(ROOT_DIR)/connectivity -I$(ROOT_DIR)/utils -I$(HAL_PATH)
LIBS = $(MBEDTLS_PATH)/library/libmbedx509.a $(MBEDTLS_PATH)/library/libmbedtls.a $(MBEDTLS_PATH)/library/libmbedcrypto.a
export INCLUDES

UTILS_OBJS = $(UTILS_PATH)/crypto_utils.o $(UTILS_PATH)/serializer.o $(UTILS_PATH)/tryte_byte_conv.o $(UTILS_PATH)/url.o $(UTILS_PATH)/tryte_msg.o $(UTILS_PATH)/logger.o
# We need to modify this rule here to be compatible to the situation
# that we have several different ways of connectivity in the future
CONNECTIVITY_OBJS = $(CONNECTIVITY_PATH)/conn_http.o

TA_OBJS = $(HTTP_PARSER_PATH)/http_parser.o $(UTILS_OBJS) $(CONNECTIVITY_OBJS)
export TA_OBJS

CFLAGS += -DTA_HOST=tangle-accel.puyuma.org -DTA_PORT=434 -DTA_API=transaction/

.SUFFIXES:.c .o
.PHONY: all clean test git_hooks hal mbedtls_make

all: git_hooks mbedtls_make hal test

mbedtls_make:
	$(MAKE) -C $(MBEDTLS_PATH)

$(CONNECTIVITY_PATH)/conn_http.o: connectivity/conn_http.c
	@echo Compiling $@ ...
	$(CC) -v -c $(CFLAGS) $(INCLUDES) -o $@ $<

$(HTTP_PARSER_PATH)/http_parser.o: $(THIRD_PARTY_PATH)/http-parser/http_parser.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

$(UTILS_OBJS):
	$(MAKE) -C $(UTILS_PATH)

test: mbedtls_make $(UTILS_OBJS) $(HTTP_PARSER_PATH)/http_parser.o $(CONNECTIVITY_PATH)/conn_http.o
	$(MAKE) -C $(TEST_PATH)

hal:
	$(MAKE) -C $(HAL_PATH)

clean: clean_client clean_third_party clean_test clean_devices

clean_test:
	$(MAKE) -C $(TEST_PATH) clean

clean_client:
	$(MAKE) -C $(UTILS_PATH) clean
	$(MAKE) -C $(CONNECTIVITY_PATH) clean
	rm -f *.o *.c.d

clean_devices:
	$(MAKE) -C $(HAL_PATH) clean

clean_third_party: clean_mbedtls clean_http_parser

clean_mbedtls:
	@for dir in $(MBEDTLS_PATH); do \
		$(MAKE) -C $$dir clean; \
		if [ $$? != 0 ]; then exit 1; fi; \
	done

clean_http_parser:
	$(MAKE) -C $(HTTP_PARSER_PATH) clean

git_hooks:
	git config core.hooksPath hooks
