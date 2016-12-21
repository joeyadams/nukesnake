PROG = nukesnake

CC = gcc
CFLAGS = -c -g -Os -Wall -W -Wwrite-strings -Wpointer-arith
LIBS = `allegro-config release --libs` -lm
LIBHEADERS = `allegro-config release --cflags`

COMPILE_C = $(CC) $(CFLAGS)
HEADERS = *.h
OBJECTS = main.o glue.o game.o ai.o layout.o menu.o

RM = rm -f

all: $(PROG)

%.o: %.c $(HEADERS)
	$(COMPILE_C) $(LIBHEADERS) $< -o $@

$(PROG): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o $@

clean:
	-$(RM) $(OBJECTS)
	-$(RM) $(PROG)
	-$(RM) *~
