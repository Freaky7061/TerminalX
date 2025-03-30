CC = g++
CFLAGS = -Wall -std=c++11
TARGET = my_shell
OBJ = main.o mycp.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

main.o: main.cpp mycp.h
	$(CC) $(CFLAGS) -c main.cpp

mycp.o: mycp.cpp mycp.h
	$(CC) $(CFLAGS) -c mycp.cpp

clean:
	rm -f $(OBJ) $(TARGET)
