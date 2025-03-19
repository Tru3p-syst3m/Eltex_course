# Определяем компилятор и флаги компиляции
CC = gcc
CFLAGS = -Wall -Werror

# Каталоги
BIN_DIR = bin
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = includes
LIB_DIR = $(SRC_DIR)/lib

# Все исходные файлы в каталоге lib
LIB_SRCS = $(wildcard $(LIB_DIR)/*.c)
# Заголовочные файлы
LIB_HDRS = $(wildcard $(INCLUDE_DIR)/*.h)
# Главный файл программы
MAIN_SRC = $(SRC_DIR)/main.c

# Преобразуем исходные файлы в объектные
LIB_OBJS = $(LIB_SRCS:$(LIB_DIR)/%.c=$(BUILD_DIR)/%.o)
MAIN_OBJ = $(BUILD_DIR)/main.o

# Статическая библиотека
LIBRARY = $(BUILD_DIR)/libcalc.a

# Цель по умолчанию
all: $(BIN_DIR)/calc

# Правило для создания статической библиотеки
$(LIBRARY): $(LIB_OBJS)
	ar rcs $@ $^

# Правило для компиляции исходных файлов библиотеки в объектные файлы
$(BUILD_DIR)/%.o: $(LIB_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDE_DIR)

# Правило для компиляции главного файла программы в объектный файл
$(MAIN_OBJ): $(MAIN_SRC)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDE_DIR)

# Линковка программы с библиотекой
$(BIN_DIR)/calc: $(MAIN_OBJ) $(LIBRARY) $(SRC_DIR)/clear_buff.c
	$(CC) $(CFLAGS) $^ -o $@ -L$(BUILD_DIR) -lcalc -I$(INCLUDE_DIR)

# Чистка всех файлов сборки
clean:
	rm -rf $(BUILD_DIR)/* $(BIN_DIR)/*

run: $(BIN_DIR)/calc
	./$(BIN_DIR)/calc

.PHONY: all clean run
