SRC_DIR = src
OBJ_DIR = build
INC_DIR = include
TARGET = webc

CC = gcc
CFLAGS = -Wall -Wextra -Oz -I$(INC_DIR)

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

all: $(TARGET)

$(TARGET): $(OBJ_FILES) 
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c 
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean
