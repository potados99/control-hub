CC = gcc
CFLAGS = -Wall -O3 -D VERBOSE
TARGET = control
BIN = /usr/local/bin

$(TARGET) : main.c serial.c setting.c
	$(CC) $(CFLAGS) -o $(TARGET) *.c
	sudo mv $(TARGET) $(BIN)/

clean:
	rm $(BIN)/$(TARGET)
