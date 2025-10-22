NAME        = cub3D
SRCS_DIR    = ./src/
OBJS_DIR    = ./obj/
LIB_DIR     = ./libs/
INCLUDE     = -I./includes -I$(LIB_DIR)minilibx-linux -I$(LIB_DIR)libft
MFLAGS      = -L$(LIB_DIR)minilibx-linux -lmlx -lXext -lX11 -lm -lz
CFLAGS      = -Wall -Werror -Wextra -g
LIBFT_PATH  = $(LIB_DIR)libft/libft.a

SRC_FILES   = main \
			  parsing/read_line \
			  parsing/process_map_lines \
			  parsing/parse_textures  \
			  parsing/parse_map \
			  parsing/parse_player \
			  debug/debug_print \
			  validations/validate_map \
			  validations/validation_utils \
			  validations/validation_map_utils \
			  loader/map_loader \
			  loader/texture_loader \
			  utils/utils \
			  utils/assign_utils \
			  utils/map_utils \
			  render/raycast


OBJS        = $(addprefix $(OBJS_DIR), $(addsuffix .o, $(SRC_FILES)))

REPOS = libft minilibx

all: $(REPOS) $(NAME)

libft:
	@echo "📚 Cloning libft only if not present"
	@if [ ! -d $(LIB_DIR)libft ]; then \
		mkdir -p $(LIB_DIR); \
		git clone https://github.com/Madirex/libft.git $(LIB_DIR)libft; \
	fi

minilibx:
	@echo "🎮 Cloning minilibx only if not present"
	@if [ ! -d $(LIB_DIR)minilibx-linux ]; then \
		mkdir -p $(LIB_DIR); \
		git clone https://github.com/42Paris/minilibx-linux.git $(LIB_DIR)minilibx-linux; \
	fi

$(NAME): $(OBJS)
	@echo "🔧 Compiling libft..."
	@make -C $(LIB_DIR)libft
	@echo "🔧 Compiling minilibx..."
	@make -C $(LIB_DIR)minilibx-linux
	@echo "🚀 Compiling $(NAME)..."
	@$(CC) $(CFLAGS) $(OBJS) $(LIBFT_PATH) $(MFLAGS) -o $(NAME)
	@echo "✅ $(NAME) compiled successfully!"

$(OBJS_DIR)%.o: $(SRCS_DIR)%.c
	@mkdir -p $(@D)
	@echo "📝 Compiling $<"
	@$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	@echo "🧹 Cleaning objects..."
	@$(RM) -r $(OBJS_DIR)
	@make clean -C $(LIB_DIR)libft 2>/dev/null || true
	@make clean -C $(LIB_DIR)minilibx-linux 2>/dev/null || true

fclean: clean
	@echo "🗑️  Full clean..."
	@$(RM) $(NAME)
	@make fclean -C $(LIB_DIR)libft 2>/dev/null || true
	@make clean -C $(LIB_DIR)minilibx-linux 2>/dev/null || true

re: fclean all

test: $(NAME)
	@echo "🧪 Starting comprehensive test suite..."
	@echo ""
	@if [ ! -d "tests" ]; then \
		echo "❌ Error: Directory 'tests' does not exist"; \
		exit 1; \
	fi
	@if [ ! -d "tests/maps" ]; then \
		echo "❌ Error: Directory 'tests/maps' does not exist"; \
		exit 1; \
	fi
	@if [ ! -d "tests/maps/good" ]; then \
		echo "❌ Error: Directory 'tests/maps/good' does not exist"; \
		exit 1; \
	fi
	@if [ ! -d "tests/maps/bad" ]; then \
		echo "❌ Error: Directory 'tests/maps/bad' does not exist"; \
		exit 1; \
	fi
	@echo "🟢 Correct cases"
	@echo "=================="
	@for map in tests/maps/good/*.cub; do \
		if [ -f "$$map" ]; then \
			echo "Testing: $$map"; \
			./$(NAME) "$$map" || echo "❌ Failed: $$map"; \
		fi; \
	done
	@if [ -z "$$(ls tests/maps/good/*.cub 2>/dev/null)" ]; then \
		echo "⚠️  No .cub files found in tests/maps/good/"; \
	fi
	@echo ""
	@echo "🔴 Incorrect cases"
	@echo "=================="
	@for map in tests/maps/bad/*.cub; do \
		if [ -f "$$map" ]; then \
			echo "Testing: $$map"; \
			./$(NAME) "$$map" && echo "⚠️  Expected failure but passed: $$map" || echo "✅ Correctly failed: $$map"; \
		fi; \
	done
	@if [ -z "$$(ls tests/maps/bad/*.cub 2>/dev/null)" ]; then \
		echo "⚠️  No .cub files found in tests/maps/bad/"; \
	fi
	@echo ""
	@echo "🏁 Test suite completed!"

.PHONY: all clean fclean re bonus libft minilibx test
