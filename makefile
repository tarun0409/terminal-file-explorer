CC = g++
HEAD_DIR = headers
OBJ_DIR = bin
SRC_DIR = src
HEAD_FILE_NAMES = default_headers.h syscalls.h
HEAD_FILES = $(patsubst %,$(HEAD_DIR)/%,$(HEAD_FILE_NAMES))
OBJ_FILE_NAMES = misc_utils.o directory.o commands.o terminal.o start_app.o
OBJ_FILES = $(patsubst %,$(OBJ_DIR)/%,$(OBJ_FILE_NAMES))
EXEC_FILE = texplore

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEAD_FILES)
	$(CC) -c -g $< -o $@

$(EXEC_FILE): $(OBJ_FILES)
	$(CC) -o $@ $^
	
clean:
	-rm $(OBJ_DIR)/*.o
