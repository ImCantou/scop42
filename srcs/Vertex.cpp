#include "Vertex.hpp"

Vertex::Vertex (void) {
}

Vertex::Vertex (glm::vec3 position): pos(position), color({0.8f, 0.8f, 0.8f}), texCoord({-1.0f, -1.0f}) {
}

Vertex::Vertex (glm::vec3 position, glm::vec2 texture, glm::vec3 normalPos): pos(position), color({0.8f, 0.8f, 0.8f}), texCoord(texture), normal(normalPos) {
	texCoord.y = 1.0f - texCoord.y;  
}

Vertex::Vertex (Vertex const & src)
{
	*this = src;
}

Vertex & Vertex::operator=(Vertex const & rhs)
{
	if (this != &rhs) {
		this->color = rhs.color;
		this->pos = rhs.pos;
		this->texCoord = rhs.texCoord;
		this->normal = rhs.normal;
	}
	return *this;
}

Vertex::~Vertex (void) {
	// std::cout << "Vertex Default Destructor Called" << std::endl;
}

VkVertexInputBindingDescription Vertex::getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
 
    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 3> Vertex::getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

	attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

    return attributeDescriptions;
}