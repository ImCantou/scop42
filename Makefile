NAME        =   Scop42

SRCS		= 	srcs/main.cpp \
				srcs/str_utils.cpp \
				srcs/Model3D.cpp \
				srcs/DeletionQueue.cpp \
				srcs/VulkanInitializer.cpp \
				srcs/PipelineCreator.cpp \
				srcs/ParserObj.cpp \
				srcs/Vertex.cpp \
				srcs/Camera3D.cpp \
				srcs/EngineUtils.cpp \
				srcs/VulkanEngine.cpp

INC_DIR		=	includes

CXX			= 	c++

RM			=	rm -f

GLM_PATH	=	external/glm

STB_PATH	=	external/stb

OBJS		=	$(SRCS:.cpp=.o)

CXXFLAGS = -Wall -Werror -Wextra -std=c++20 -O3

LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

all: ${NAME}

%.o: %.cpp 
			$(CXX) $(CXXFLAGS) -I $(INC_DIR) -I $(GLM_PATH) -I $(STB_PATH) -o $@ -c $< 

${NAME}: $(OBJS)
#	~/glslc shaders/shader.vert -o compiled_shaders/vert.spv
#	~/glslc shaders/shader.frag -o compiled_shaders/frag.spv
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) -o $(NAME) 

shaders:


test: ${NAME}
	./${NAME}

clean:
	${RM} $(OBJS)

fclean:	clean
	$(RM) $(NAME)

re: fclean all

.PHONY: test clean fclean re all
