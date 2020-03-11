DIR_BUILD = build
MBEDTLS_LIBRARY = -L$(MBEDTLS_PATH)/library
MBEDTLS_LDFLAGS = -lmbedtls -lmbedx509 -lmbedcrypto

all: create_dir build/test_tryte_byte_conv build/test_crypto_utils build/test_serializer build/test_driver

create_dir:
	[ -d $(DIR_BUILD) ] || mkdir -p $(DIR_BUILD)

build/test_tryte_byte_conv: test_tryte_byte_conv.c $(UTILS_PATH)/tryte_byte_conv.c
	$(CC) -g -DDEBUG $(INCLUDES) -o $@ $^

build/test_crypto_utils: test_crypto_utils.c $(UTILS_PATH)/crypto_utils.c
	$(CC) -g -DDEBUG $(INCLUDES) -o $@ $^ $(MBEDTLS_LIBRARY) $(MBEDTLS_LDFLAGS)

build/test_serializer: test_serializer.c $(UTILS_PATH)/serializer.c
	$(CC) -g -DDEBUG $(INCLUDES) -o $@ $^

build/test_driver: test_driver.c $(DEVICES_PATH)/emulator/emulator.o $(HAL_PATH)/device.o use_models.h
	$(CC) -g -DDEBUG $(INCLUDES) -o $@ $^ $(TA_OBJS) $(MBEDTLS_LIBRARY) $(MBEDTLS_LDFLAGS)

use_models.h:
	@cat ../template.inc > use_models.h
	@sed -i 									  \
		 -e 's/{% device_name %}/emulator/g' 	  \
		 -e 's/{% uart_port %}/\/dev\/emulator/g' \
		 -e 's/{% log_path %}/ta_endpoint.log/g'  \
	     -i use_models.h

clean:
	rm -f *.o
	rm -f build/*
	rm -f use_models.h
