CC=/compilation/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-gcc
CFLAGS=-Wall -static -I. -fpic -march=armv7-a -mfloat-abi=softfp -mfpu=vfp
SRCS = aes.c des.c md5.c helpfunctions.c emulator.c OSEmu.c
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)
BIN = osemu

all: osemu

-include $(OBJS:.o=.d)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
	$(CC) -MM $(CFLAGS) $*.c > $*.d

osemu: $(OBJS)
	$(CC) -O2 -o $(BIN) $(OBJS) $(CFLAGS)
	/compilation/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-strip $(BIN)
	
clean:
	rm -rf $(BIN) $(OBJS) $(DEPS)
	
.PHONY: osemu
