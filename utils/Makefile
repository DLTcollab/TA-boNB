UTILS_OBJS = crypto_utils.o serializer.o tryte_byte_conv.o url.o tryte_msg.o logger.o

all: $(UTILS_OBJS)
%.o: %.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $^

clean:
	rm -f *.o
