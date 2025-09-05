#Andrew Smith G01373154
#CS 262, Lab section 211
#Lab 3
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic-errors
LIBS = -lm
TARGET = p2_asmit93_211
all: $(TARGET).c
	$(CC) $(TARGET).c -o $(TARGET) $(CFLAGS) $(LIBS)
clean:
	rm $(TARGET)
run:
	./$(TARGET)

