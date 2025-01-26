CXX = g++
CFLAGS = -std=c++2a -g -Wall -fsanitize=address -fsanitize=leak
LDFLAGS = -fsanitize=address -fsanitize=leak
OBJ_DIR = obj
DEP_DIR = dep
BIN_DIR = bin

# 自动查找所有源文件
SRCS = clientTest.cpp Client/tcpClient.cpp
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEPS = $(SRCS:%.cpp=$(DEP_DIR)/%.d)
TARGETS = $(BIN_DIR)/clientTest 

all: $(TARGETS)

# 自动生成 target 规则
$(BIN_DIR)/clientTest : $(OBJ_DIR)/clientTest.o $(OBJ_DIR)/tcpClient.o | $(BIN_DIR)
	$(CXX) $(CFLAGS) $(OBJ_DIR)/clientTest.o $(OBJ_DIR)/tcpClient.o -o $(BIN_DIR)/clientTest $(LDFLAGS)

# 编译 .o 文件并生成依赖文件
$(OBJ_DIR)/tcpClient.o: Client/tcpClient.cpp | $(OBJ_DIR) $(DEP_DIR)
	$(CXX) $(CFLAGS) -MMD -MP -c Client/tcpClient.cpp -o $(OBJ_DIR)/tcpClient.o
	mv $(OBJ_DIR)/tcpClient.d $(DEP_DIR)/

$(OBJ_DIR)/clientTest.o: clientTest.cpp | $(OBJ_DIR) $(DEP_DIR)
	$(CXX) $(CFLAGS) -MMD -MP -c clientTest.cpp -o $(OBJ_DIR)/clientTest.o
	mv $(OBJ_DIR)/clientTest.d $(DEP_DIR)/

# 自动创建目录
$(OBJ_DIR) $(DEP_DIR) $(BIN_DIR):
	mkdir -p $@

# 清理构建生成的文件
clean:
	$(RM) -r $(BIN_DIR) $(OBJ_DIR) $(DEP_DIR)

# 包含依赖文件
-include $(DEPS)

.PHONY: all clean
