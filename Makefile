CC=gcc
CFLAGS=-Wall -g

TARGET=allocado
TEST_TARGET=allocado_test
SRC=$(wildcard *.c)

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

test: CFLAGS += -DTESTING
test: $(TEST_TARGET)
	./$(TEST_TARGET)
	rm -f $(TEST_TARGET)

$(TEST_TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(SRC)

clean:
	rm -f $(TARGET) $(TEST_TARGET)