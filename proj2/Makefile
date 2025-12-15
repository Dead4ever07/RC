CC = gcc
CFLAGS = -Wall -Wextra -O2

SRC_DIR = src
BIN_DIR = bin
TARGET = app

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BIN_DIR)/%.o)

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: all 
	./$(BIN_DIR)/$(TARGET) $(filter-out $@,$(MAKECMDGOALS))

%:
	@:

clean:
	rm -rf $(BIN_DIR)

rebuild: clean all
