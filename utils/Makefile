OBJS = crypto_utils.o serializer.o tryte_byte_conv.o uart_utils.o url.o

ifeq ($(DEBUG), n)
UTILS_CFLAGS = $(CFLAGS)
else
UTILS_CFLAGS = $(CFLAGS) -DKEY_DEBUG
endif

all: $(OBJS)

%.o: %.c
	$(CC) -fPIC -c $(UTILS_CFLAGS) $(INCLUDES) -o $@ $^

clean:
	rm -f *.o
