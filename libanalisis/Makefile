CC = gcc
CFLAGS = -fPIC -Wall
LDFLAGS = -shared

SRC = cJSON.c
OBJ = cJSON.o
LIBRARY = libcjson.so

all: $(LIBRARY)

$(LIBRARY): $(OBJ)
	$(CC) $(LDFLAGS) -o $(LIBRARY) $(OBJ)

clean:
	rm -f $(OBJ) $(LIBRARY)

