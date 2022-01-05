CC = g++
TARGET = main

BUILD_DIR = bin

#源文件列表
C_SOURCES = \
System_manager.cpp \
test.cpp

#链接所需库
CFLAGS = $(C_INCLUDES) -std=c++17

#执行操作
all: $(BUILD_DIR)/$(TARGET).exe 

#对象文件
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.cpp=.o)))
vpath %.cpp $(sort $(dir $(C_SOURCES)))

#执行生成 .o 文件
$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) $< -o $@

#执行生成可执行文件
$(BUILD_DIR)/$(TARGET).exe: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(Lib_INCLUDES) $(CFLAGS) -o $@ -g

#创建目录
$(BUILD_DIR):
	mkdir $@