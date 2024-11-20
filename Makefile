RGBS_INCDIR=rgbserver/include
RGBS_LIBDIR=rgbserver/lib

EXE=neddy
CC=cc
CFLAGS=-Wall -O3 -g -Wextra -Wno-unused-parameter
INCLUDE=$(RGBS_INCDIR)
LDFLAGS=-L$(RGBS_LIBDIR) -lm -lrgbserver

all: $(EXE)

$(EXE): $(EXE).o screen.o sprite.o
	$(CC) $^ -o $@ $(LDFLAGS)

%.o : %.c
	$(CC) -I$(INCLUDE) $(CFLAGS) -c -o $@ $<

clean:
	rm $(EXE) *.o
