# ====== Config ======
NAME      := webserv
CXX       := c++
RM        := rm -rf
CXXFLAGS  := -Wall -Wextra -Werror -std=c++98 -Iincludes -g

SRCS_DIR   := srcs
HEAD_DIR   := includes
OBJS_DIR   := objs

SRCS      := $(shell find $(SRCS_DIR) -name '*.cpp')
OBJS      := $(patsubst $(SRCS_DIR)/%.cpp,$(OBJS_DIR)/%.o,$(SRCS))

# ====== Rules ======
.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

# 객체 파일 빌드 규칙
$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 임시 파일 삭제
clean:
	$(RM) $(OBJS_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all