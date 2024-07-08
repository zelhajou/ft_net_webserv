CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
SRC_DIR = src
BUILD_DIR = build
TARGET = webserv

SRCS = $(wildcard $(SRC_DIR)/**/*.cpp $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

all: $(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

fclean: clean

re: fclean all

.PHONY: all clean fclean re
