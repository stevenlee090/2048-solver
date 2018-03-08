CC=gcc
CPPFLAGS=-Wall
CPPFLAGS=-g

# Define the compiler flags
#  -Wall 		to turn on most of the warnings
#  -Wextra 		to turn on even more warnings
#  -Wpedantic 		to turn on even more warnings
#  -ansi 		to ensure ansi compliance
#  -std=c99 		to ensure c99 compliance
#  -m32 		to compile for 32 bit architectures
#  -O0 			to turn off optimisations
#  -g  			to include debugging symbols
#  -Werror 		to treat all warnings as errors
#  -O3			to turn on optimisations

SRC=src/utils.o src/priority_queue.o src/ai.o src/2048.o
TARGET=2048

all: $(SRC)
	$(CC) -o $(TARGET) $(SRC) $(CPPFLAGS)

clean:
	rm -f $(TARGET) src/*.o
