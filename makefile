CFLAGS = `sdl2-config --cflags` -O2 -Wall -Wextra -pedantic -Wfloat-equal -ansi -lm
INCS = neillsdl2.h test.h parse.h 
TARGET = parse
SOURCES =  $(TARGET).c neillsdl2.c test.c symbol.c stack.c
LIBS =  `sdl2-config --libs` -lm
CC = gcc

all: $(TARGET) interp

$(TARGET): $(SOURCES) $(INCS)
	$(CC) $(SOURCES) -o $(TARGET) $(CFLAGS) $(LIBS)

interp: interp.c prog.c neillsdl2.c bst.c stack.c bst_test.c helper.c bst_test.h neillsdl2.h interp.h bst.h
	$(CC) interp.c prog.c neillsdl2.c bst.c stack.c bst_test.c helper.c -o interp $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET) ./interp 
