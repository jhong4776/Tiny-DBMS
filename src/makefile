CC = g++
TARGET = main

BUILD_DIR = bin
FINAL_DIR = final

SRC=./Parser-manager/third_party/antlr4-cpp-runtime-4.7.2/src/
ATN=./Parser-manager/third_party/antlr4-cpp-runtime-4.7.2/src/atn/
DFA=./Parser-manager/third_party/antlr4-cpp-runtime-4.7.2/src/dfa/
MISC=./Parser-manager/third_party/antlr4-cpp-runtime-4.7.2/src/misc/
SUPPORT=./Parser-manager/third_party/antlr4-cpp-runtime-4.7.2/src/support/
TREE=./Parser-manager/third_party/antlr4-cpp-runtime-4.7.2/src/tree/
PATTERN=./Parser-manager/third_party/antlr4-cpp-runtime-4.7.2/src/tree/pattern/
XPATH=./Parser-manager/third_party/antlr4-cpp-runtime-4.7.2/src/tree/xpath/
GENERATED=./Parser-manager/generated/

#源文件列表
C_SOURCES = \
test/test.cpp \
System-manager/System_manager.cpp \
Index-manager/BplusTree.cpp \
Index-manager/Index_manager.cpp \
Page-manager/Header_manager.cpp \
Parser-manager/Parser_manager.cpp \
Record-manager/Record_manager.cpp \
Check-manager/Check_manager.cpp \
$(wildcard ./*cpp $(SRC)*.cpp) \
$(wildcard ./*cpp $(GENERATED)*.cpp) \
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
all: $(FINAL_DIR)/$(TARGET).exe 

#对象文件
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.cpp=.o)))
vpath %.cpp $(sort $(dir $(C_SOURCES)))

#执行生成 .o 文件
$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) $< -o $@

#执行生成可执行文件
$(FINAL_DIR)/$(TARGET).exe: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(Lib_INCLUDES) $(CFLAGS) -o $@ -g

#创建目录
$(BUILD_DIR):
	mkdir $@