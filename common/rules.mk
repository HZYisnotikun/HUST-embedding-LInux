
CROSS_COMPILE:=/home/emb/Documents/gcc-arm-9.2-2019.12-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-

CC:=$(CROSS_COMPILE)gcc

CFLAGS := -Wall -O3 -funroll-loops -fprefetch-loop-arrays -falign-functions=32
LDFLAGS:=-Wall

INCLUDE := -I../common/external/include
LIB := -L../common/external/lib -ljpeg -lfreetype -lpng -lasound -lz -lc -lm -pthread

EXESRCS := ../common/graphic.c ../common/touch.c ../common/image.c ../common/task.c $(EXESRCS)

EXEOBJS := $(patsubst %.c, %.o, $(EXESRCS))

$(EXENAME): $(EXEOBJS)
	$(CC) $(LDFLAGS) -o $(EXENAME) $(EXEOBJS) $(LIB)
	mv $(EXENAME) ../out/

clean:
	rm -f $(EXENAME) $(EXEOBJS)

%.o: %.c ../common/common.h
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<

