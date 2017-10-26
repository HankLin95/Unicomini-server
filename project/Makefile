BIN=main maind
CXX=g++ -g
CFLAGS=-Wall -g
SRC=$(wildcard *.cpp)
OBJECTS=$(SRC:.cpp=.o)
main:$(OBJECTS)
	$(CC) $^ -o $@ -lpthread -lrt -lstdc++ -lsqlite3
maind:$(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@
clean:
	rm -f *.o $(BIN)
