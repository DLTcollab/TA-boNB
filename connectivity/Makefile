all: conn_http.o

.NOTPARALLEL:
connectivity/conn_http.o: conn_http.c
	@echo Compiling $@ ...
	$(CC) -v -c $(CFLAGS) $(INCLUDES) -MMD -MF conn_http.c.d -o $@ $<

clean:
	rm -f *.o *.c.d

-include conn_http.c.d
