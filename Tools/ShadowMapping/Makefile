CC = g++
AS = as
CFLAGS = -std=c++20 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 

TARGET = Test
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(CFLAGS) 

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)