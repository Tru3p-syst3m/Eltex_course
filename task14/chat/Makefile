# Компилятор
CC = gcc

# Флаги компиляции
CFLAGS = -Wall -Wextra -Iinclude -g -O0

# Директории
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

# Исходные файлы
CLIENT_SRC = $(SRC_DIR)/client.c
SERVER_SRC = $(SRC_DIR)/server.c
DATA_SRC = $(SRC_DIR)/data.c

# Объектные файлы
CLIENT_OBJ = $(BUILD_DIR)/client.o
SERVER_OBJ = $(BUILD_DIR)/server.o
DATA_OBJ = $(BUILD_DIR)/data.o

# Исполняемые файлы
CLIENT_EXE = client
SERVER_EXE = server

# Правило по умолчанию
all: $(BUILD_DIR) $(CLIENT_EXE) $(SERVER_EXE)

# Создание директории build, если её нет
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Компиляция клиента (линкуется с data.o, если нужно)
$(CLIENT_EXE): $(CLIENT_OBJ) $(DATA_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lncurses

# Компиляция сервера (линкуется с data.o)
$(SERVER_EXE): $(SERVER_OBJ) $(DATA_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lncurses

# Правила для объектных файлов
$(BUILD_DIR)/client.o: $(CLIENT_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/server.o: $(SERVER_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/data.o: $(DATA_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

# Очистка
clean:
	rm -rf $(BUILD_DIR) $(CLIENT_EXE) $(SERVER_EXE)

.PHONY: all clean