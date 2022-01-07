CC = g++
TARGET = main

BUILD_DIR = bin

HEADER += -I./third_party

SRC=./third_party/antlr4-cpp-runtime-4.7.2/src/
ATN=./third_party/antlr4-cpp-runtime-4.7.2/src/atn/
DFA=./third_party/antlr4-cpp-runtime-4.7.2/src/dfa/
MISC=./third_party/antlr4-cpp-runtime-4.7.2/src/misc/
SUPPORT=./third_party/antlr4-cpp-runtime-4.7.2/src/support/
TREE=./third_party/antlr4-cpp-runtime-4.7.2/src/tree/
PATTERN=./third_party/antlr4-cpp-runtime-4.7.2/src/tree/pattern/
XPATH=./third_party/antlr4-cpp-runtime-4.7.2/src/tree/xpath/

#源文件列表
C_SOURCES = \
src/main.cpp \
generated/SQLLexer.cpp \
generated/SQLParser.cpp \
generated/SQLVisitor.cpp \
generated/MyVisitor.cpp \
$(wildcard ./*cpp $(SRC)*.cpp) \
$(wildcard ./*cpp $(ATN)*.cpp) \
$(wildcard ./*cpp $(DFA)*.cpp) \
$(wildcard ./*cpp $(XPATH)*.cpp) \
$(wildcard ./*cpp $(MISC)*.cpp) \
$(wildcard ./*cpp $(SUPPORT)*.cpp) \
$(wildcard ./*cpp $(TREE)*.cpp) \
$(wildcard ./*cpp $(PATTERN)*.cpp)

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