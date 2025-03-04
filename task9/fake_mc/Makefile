# Устанавливаем переменные для компилятора и флагов
CC = gcc
CFLAGS = -Wall -g -Iincludes  # -Iincludes для добавления пути к заголовочным файлам
LDFLAGS = -lncurses # Здесь можно указать флаги для линковки, если необходимо

# Папки для исходных, объектных файлов и бинарников
SRC_DIR = src
OBJ_DIR = build
BIN_DIR = bin
INCLUDE_DIR = includes

# Находим все исходные файлы .c в src/
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
# Для каждого исходного файла создаём объектный файл в build/
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
# Имя исполнимого файла
EXEC = $(BIN_DIR)/fake_mc

# Главная цель по умолчанию
all: $(EXEC)

# Линковка объектных файлов в исполнимый файл
$(EXEC): $(OBJ_FILES)
	$(CC) -O0 -g $(OBJ_FILES) -o $(EXEC) $(LDFLAGS)

# Правило для компиляции исходных файлов в объектные
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -O0 -g $(CFLAGS) -c $< -o $@

# Очистка временных и скомпилированных файлов
clean:
	rm -rf $(OBJ_DIR)/*.o $(EXEC)

# Правило для создания каталога build, если его нет
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Правило для создания каталога bin, если его нет
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Перезапуск сборки (например, для нового кода)
rebuild: clean all

run: 
	./$(EXEC)

dbg:
	gdb ./$(EXEC)
	
