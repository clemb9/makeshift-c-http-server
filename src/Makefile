IDIR = ../include
ODIR = obj
LDIR = ../lib

CC=gcc

CFLAGS=-Wall -I$(IDIR) 

LIBS=-pthread -lmagic

_DEPS = utils.h
_OBJ = error.o wrappers.o engine.o tools.o signals.o responses.o

DEPS = $(patsubst %, $(IDIR)/%, $(_DEPS))
OBJ = $(patsubst %, $(ODIR)/%, $(_OBJ))

CLI_OBJ = obj/client.o $(OBJ)
SRV_OBJ = obj/server.o $(OBJ)

all: client server

client: $(CLI_OBJ)
	$(CC) -o $@ $^ $(LIBS)

server: $(SRV_OBJ)
	$(CC) -o $@ $^ $(LIBS)

$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

clean: 
	rm -f $(ODIR)/*.o

.PHONY: all clean
