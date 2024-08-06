CXX = c++ -g -fsanitize=address 
#CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Iinclude
CXXFLAGS = -Iinclude
SRC_DIR = src
BUILD_DIR = build
TARGET = webserv

SRCS = $(wildcard $(SRC_DIR)/**/*.cpp $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

all: $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# expand:
# 	@rm -rf /tmp/2.conf /tmp/Sockets.hpp
# 	@envsubst < ./config/config_files/2.conf > /tmp/2.conf && cat < /tmp/2.conf > config/config_files/2.conf
# 	@envsubst < ./include/Sockets.hpp > /tmp/Sockets.hpp && cat < /tmp/Sockets.hpp > include/Sockets.hpp

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

fclean: clean

re: fclean all

.PHONY: all clean fclean re
