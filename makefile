CXX = g++
CFLAGS = -std=c++2a -g -Wall -fsanitize=address -fsanitize=leak
LDFLAGS = -fsanitize=address -fsanitize=leak
OBJ_DIR = obj
DEP_DIR = dep
BIN_DIR = bin
SRC_DIR = source
TEST_DIR = test

# 自动查找所有源文件
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEPS = $(SRCS:%.cpp=$(DEP_DIR)/%.d)
TARGETS = $(BIN_DIR)/clientTest $(BIN_DIR)/serverTest 

all: $(TARGETS)

# 自动生成 target 规则
# clientTest
$(BIN_DIR)/clientTest : $(OBJ_DIR)/clientTest.o $(OBJ_DIR)/tcpClient.o | $(BIN_DIR)
	$(CXX) $(CFLAGS) $(OBJ_DIR)/clientTest.o $(OBJ_DIR)/tcpClient.o -o $(BIN_DIR)/clientTest $(LDFLAGS)

#serverTest
$(BIN_DIR)/serverTest : $(OBJ_DIR)/serverTest.o $(OBJ_DIR)/tcpServer.o | $(BIN_DIR)
	$(CXX) $(CFLAGS) $(OBJ_DIR)/serverTest.o $(OBJ_DIR)/tcpServer.o -o $(BIN_DIR)/serverTest $(LDFLAGS)

# 编译 .o 文件并生成依赖文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR) $(DEP_DIR)
	$(CXX) $(CFLAGS) -MMD -MP -c $< -o $@
	mv $(@:.o=.d) $(DEP_DIR)/

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp | $(OBJ_DIR) $(DEP_DIR)
	$(CXX) $(CFLAGS) -MMD -MP -c $< -o $@
	mv $(@:.o=.d) $(DEP_DIR)/
	

# 自动创建目录
$(OBJ_DIR) $(DEP_DIR) $(BIN_DIR):
	mkdir -p $@

# 清理构建生成的文件
clean:
	$(RM) -r $(BIN_DIR) $(OBJ_DIR) $(DEP_DIR)

# 包含依赖文件
-include $(DEPS)

.PHONY: all clean
