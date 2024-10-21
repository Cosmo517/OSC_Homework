BINARY = custom_shell

CC = gcc
CFLAGS = -Wall -g

SRC = main.c

TARGET = $(BINARY)

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

install: $(TARGET)
	sudo cp $(TARGET) /bin/

uninstall:
	sudo rm -f /bin/$(TARGET)

clean:
	rm -f $(TARGET)
