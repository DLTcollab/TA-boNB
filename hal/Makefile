DEVICES_OBJ = wp7702.o emulator.o device.o
INCLUDES += -I/$(CURDIR)
export DEVICES_OBJ

all: $(DEVICES_OBJ)

wp7702.o: device.o
	$(MAKE) -C ../devices/sierra_wireless/wp7702

emulator.o: device.o
	$(MAKE) -C ../devices/emulator

device.o: device.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $^

clean:
	find $(DEVICES_PATH) -name "*.o" -exec rm -f {} \;
	rm *.o
