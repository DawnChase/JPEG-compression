# 编译器及编译参数
CC = g++
CFLAGS = -std=c++11 -I./h

# 目标文件名
ifeq ($(OS),Windows_NT)
    TARGET = main.exe
    RM = del /F /Q
else
    TARGET = main
    RM = rm -f
endif

# 源文件路径
SRC = src/main.cpp src/h/encode.cpp src/h/decode.cpp src/h/bmp.cpp src/h/head.cpp src/h/huffman.cpp

# 默认目标
all: $(TARGET)

# 编译链接生成目标文件
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# 清理生成的文件
clean:
	$(RM) $(TARGET)