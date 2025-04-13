# Компилятор
CC = gcc
# Флаги компиляции
CFLAGS = -Wall -Wextra -Werror -I./include
# Флаги линковки
LDFLAGS = 

# Исходные файлы (находятся в папке src)
SRC_DIR = src
# Объектные файлы (будут помещены в папку obj)
OBJ_DIR = obj
# Исполняемый файл (будет помещен в папку bin)
BIN_DIR = bin

# Получаем список всех .c файлов в папке src
SOURCES = $(wildcard $(SRC_DIR)/*.c)
# Генерируем список объектных файлов (заменяем .c на .o и добавляем папку obj)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
# Имя исполняемого файла
TARGET = $(BIN_DIR)/simple_bash

# Стандартная цель по умолчанию
all: $(TARGET)

# Правило для сборки исполняемого файла
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ -o $@

# Правило для компиляции .c файлов в .o файлы
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Создаем папки, если они не существуют
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Очистка - удаляем объектные файлы и исполняемый файл
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Пересобираем проект
rebuild: clean all

run: 
	./$(TARGET)

.PHONY: all clean rebuild