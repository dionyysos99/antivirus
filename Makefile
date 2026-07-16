CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -lcrypto

SRC_DIR = src
OBJ_DIR = obj

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

TARGET = antivirus

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
TEST_SRCS = $(filter-out $(SRC_DIR)/main.c, $(SRCS))

test: $(TEST_SRCS) test_main.c
	$(CC) $(CFLAGS) $(TEST_SRCS) test_main.c -o test_runner $(LDFLAGS)
re: clean all

.PHONY: all clean re