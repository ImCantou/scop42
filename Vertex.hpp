#ifndef __VERTEX_CLASS__
#define __VERTEX_CLASS__

# include <iostream>
# include <array>

#define	GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>


#include "external/glm/glm/glm.hpp"
#include "external/glm/glm/gtc/matrix_transform.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>

class Vertex {

	private:
		glm::vec3	pos;
    	glm::vec3	color;
		glm::vec3	normal;
		glm::vec2	texCoord;


	public:
		Vertex(void);
		Vertex(glm::vec3 position);
		Vertex(const Vertex & src);
		~Vertex(void);

		Vertex & operator=(Vertex const & rhs);

    	static VkVertexInputBindingDescription getBindingDescription();
    	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
};

#endif