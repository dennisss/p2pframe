BUILD_PATH = bin
#OBJ_PATH = obj
CC = gcc
AR = ar
CFLAGS += -fpermissive -pthread

libp2pframe_TARGET = bin/libp2pframe.a
libp2pframe_SRCS = src/p2pserver.o src/p2pclient.o src/p2pstate.o src/p2pproto.o

p2pframe_TARGET = bin/p2pframe
p2pframe_SRCS = src/p2pframe.o


#Prevent deletion of objects on every run
.SECONDARY: $(wildcard *.o)

p2pframe: $(p2pframe_SRCS) libp2pframe
	$(CC) $(CFLAGS) -o $(p2pframe_TARGET) $< -static $(libp2pframe_TARGET)

libp2pframe: $(libp2pframe_SRCS)
	$(AR) rcs $(libp2pframe_TARGET) $?

bin/%: tests/%.o libp2pframe
	$(CC) $(CFLAGS) -o $@ $< -static $(libp2pframe_TARGET)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $?

clean:
	rm -f src/*.o
	rm -f tests/*.o
	rm -f bin/*
