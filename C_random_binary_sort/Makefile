CC = gcc
CFLAGS = -O3 -Wall -Wextra -std=c99 -pedantic -D_POSIX_C_SOURCE=199309L

TARGET = benchmark

all: $(TARGET)

$(TARGET): benchmark.o random_binary_sort.o
	$(CC) $(CFLAGS) -o $@ $^

benchmark.o: benchmark.c random_binary_sort.h
	$(CC) $(CFLAGS) -c $<

random_binary_sort.o: random_binary_sort.c random_binary_sort.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run