CXX = g++
CFLAGS = -std=c++2a -g -Wall
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
TARGETS = $(BIN_DIR)/httpTest


all: $(TARGETS)

# 自动生成 target 规则

$(BIN_DIR)/httpTest :$(OBJ_DIR)/httpTest.o $(OBJ_DIR)/buffer.o $(OBJ_DIR)/epoll.o $(OBJ_DIR)/httpConn.o $(OBJ_DIR)/httpRequest.o $(OBJ_DIR)/httpResponse.o $(OBJ_DIR)/log.o $(OBJ_DIR)/server.o $(OBJ_DIR)/tcpListener.o $(OBJ_DIR)/threadPool.o | $(BIN_DIR)
	$(CXX) $(CFLAGS)  $(OBJ_DIR)/httpTest.o $(OBJ_DIR)/buffer.o $(OBJ_DIR)/epoll.o $(OBJ_DIR)/httpConn.o $(OBJ_DIR)/httpRequest.o $(OBJ_DIR)/httpResponse.o $(OBJ_DIR)/log.o $(OBJ_DIR)/server.o $(OBJ_DIR)/tcpListener.o $(OBJ_DIR)/threadPool.o -o $(BIN_DIR)/httpTest $(LDFLAGS)

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
