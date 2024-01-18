#pragma once

 #define GLFW_INCLUDE_VULKAN
 #include <GLFW/glfw3.h>
 #define GLFW_EXPOSE_NATIVE_X11
 #include <GLFW/glfw3native.h>

 #include <vector>
 #include <optional>

 #include "Camera3D.hpp"

 #define WIDTH 800
 #define HEIGHT 600
 #define MAX_FRAMES_IN_FLIGHT 2

 #ifdef NDEBUG
    #define enableValidationLayers false
 #else
    #define enableValidationLayers true
 #endif

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {return graphicsFamily.has_value() && presentFamily.has_value();}
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR 		capabilities;
    std::vector<VkSurfaceFormatKHR>	formats;
    std::vector<VkPresentModeKHR>	presentModes;
};

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

