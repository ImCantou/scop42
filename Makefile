
SRCS= main.cpp

CXX= g++

NAME        =   VulkanTest

MLXDIR		=	minilibx

MLX			=	minilibx/libmlx_Linux.a

GLM_PATH	=	external/glm

STB_PATH	=	external/stb

all: ${NAME}

${MLX}:
		@make --silent -C ${MLXDIR}
		@printf "`tput bold`\033[92m[SUCCESS]\033[0;37m: `tput bold`Compilating \033[1;94m${MLXDIR}\033[0;37m/$(notdir $(MLX))\033[1;37m to library done.\n`tput sgr0`"


CXXFLAGS = -std=c++20 -O3 -I$(GLM_PATH) -I$(STB_PATH)

LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi


$(OBJDIR)/%.o: $(SRCDIR)/%.c 
			@$(CC) -c $(CFLAGS) $< -o $@
			@printf "`tput bold`\033[92m[SUCCESS]\033[0;37m: `tput bold`Compilating \033[1;94m${SRCDIR}\033[0;37m/$(notdir $<)\033[1;37m to object done.\n`tput sgr0`"



${NAME}: $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(NAME) main.cpp $(LDFLAGS)

shaders:
	~/glslc/bin/glslc shaders/shader.vert -o compiled_shaders/vert.spv
	~/glslc/bin/glslc shaders/shader.frag -o compiled_shaders/frag.spv

debug: $(SRCS)
	$(CXX) $(CXXFLAGS) -DNDEBUG -o $(NAME) main.cpp $(LDFLAGS)

test: ${NAME}
	./VulkanTest

clean:
		rm -f $(NAME)

re: clean all

.PHONY: test clean
