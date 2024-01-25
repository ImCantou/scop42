#pragma once



#include "EngineUtils.hpp"

#include "ParserObj.hpp"

#include "VulkanInitializer.hpp"

#include "PipelineCreator.hpp"

#include "DeletionQueue.hpp"

#include "AllocatedBuffer.hpp"


class VulkanEngine {
public:
	VulkanEngine(std::string path);

    void	run();


	// Setters for callbacks
	void	setCameraZoom(double offset);
	void	setMousePos(double x, double y);
	void	changeWireframeMode();
	void	resize();

private:
	//Window basic components
	GLFWwindow* 					window;
	VkInstance 						instance;
	
	//Validation Layers manager (print message when vulkan not used properly)
	VkDebugUtilsMessengerEXT		debugMessenger;

	//Physical and logical device (GPU)
	VkPhysicalDevice 				physicalDevice;
	VkDevice 						device;

	// Queues 
	VkQueue 						graphicsQueue;
    VkQueue							presentQueue;

	// ???
	VkSurfaceKHR					surface;

	//Swap chain and Images to render (+ various images infos)
	VkSwapchainKHR					swapChain;
	std::vector<VkImage>			swapChainImages;
	VkFormat						swapChainImageFormat;
	std::vector<VkImageView>		swapChainImageViews;
	VkExtent2D						swapChainExtent;
	std::vector<VkFramebuffer>		swapChainFramebuffers;

	// graphical Pipeline (manage the different stages of rendering an image)
	VkRenderPass 					renderPass;
	VkDescriptorSetLayout			descriptorSetLayout;
	VkDescriptorPool				descriptorPool;
	std::vector<VkDescriptorSet>	descriptorSets;
	VkPipelineLayout				pipelineLayout;
	VkPipeline						graphicsPipeline;
	VkPipeline						graphicsPipelineWireframe;

	// Command pool and buffer to add to Queues
	VkCommandPool					commandPool;
	std::vector<VkCommandBuffer>	commandBuffers;

	//Buffers and GPU memory space

	t_AllocatedBuffer				vertexABuffer;
	t_AllocatedBuffer				indexABuffer;
	std::vector<t_AllocatedBuffer>	uniformABuffers;
	std::vector<void*> 				uniformBuffersMapped;


	//Semaphores and fences to synchronize CPU and GPU
	std::vector<VkSemaphore> 		imageAvailableSemaphores;
	std::vector<VkSemaphore>		renderFinishedSemaphores;
	std::vector<VkFence>			inFlightFences;

	// Textures Image storage
	VkImage 						textureImage;
	VkDeviceMemory					textureImageMemory;
	VkImageView						textureImageView;
	VkSampler						textureSampler;

	//	DepthBuffering Resources
	VkImage							depthImage;
	VkDeviceMemory					depthImageMemory;
	VkImageView						depthImageView;

	//	Model definition
	std::string						pathToFile;
	Model3D							model;

	//	Extensions and validationLayers
	const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
	const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	// DeletionQueue For Vulkan Components on cleanup
	DeletionQueue					_mainDeletionQueue;

	//Basic variables
	uint32_t						currentFrame;
	bool							isInitialized;
	bool							wireframeMode;
	bool							framebufferResized;
	Camera3D						camera;
	glm::vec2						mousePos;
	glm::vec2						rotationOffset;

	///                            Private Methods                                ///

	// Execution and cleanup
	void mainLoop();
	void drawFrame();
	void cleanup();
	void cleanupSwapChain();


	//GLFW window initialisation
	void initWindow();
	
	//Event handlers
	static void	scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void	mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	//Vulkan initialisation
	void initVulkan();
	
	void	createInstance();
	void	setupDebugMessenger();
	void 	createSurface();

	void 	pickPhysicalDevice();
	void	createLogicalDevice();
	
	void	createSwapChain();
	void	createImageViews();
	void	createRenderPass();
	
	void 	createDescriptorSetLayout();
	void	createGraphicsPipeline();
	void	createCommandPool();

	void	createDepthResources();
	void	createFramebuffers();
	
	void	createTextureImage();
	void 	createTextureImageView();
	void 	createTextureSampler();

	void	loadModel();

	void 	createVertexBuffer();
	void 	createIndexBuffer();
	void 	createUniformBuffers();
	void 	createCommandBuffer();

	void 	createDescriptorPool();
	void 	createDescriptorSets();
	
	void 	createSyncObjects();


	// Utils
	VkFormat					findDepthFormat();
	VkFormat					findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	bool 						hasStencilComponent(VkFormat format);
	VkImageView					createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	void						createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags props, VkImage& image, VkDeviceMemory& imageMemory);
	void						transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void						copyBufferToImage(VkBuffer buffer, VkImage image, u_int32_t width, uint32_t height);
	void						copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);
	VkCommandBuffer				beginSingleTimeCommands();
	void 						endSingleTimeCommands(VkCommandBuffer commandBuffer);
	uint32_t					findMemoryType(uint32_t typefilter, VkMemoryPropertyFlags properties);
	void						createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory &bufferMemory);
	void						recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	VkShaderModule				createShaderModule(const std::vector<char>& code);
	bool 						isDeviceSuitable(VkPhysicalDevice device);
	bool 						checkDeviceExtensionSupport(VkPhysicalDevice device);
	QueueFamilyIndices			findQueueFamilies(VkPhysicalDevice device);
	SwapChainSupportDetails		querySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR 			chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR 			chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D 					chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	void						populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void						updateUniformBuffer(uint32_t currentImage);
	void						recreateSwapChain();
	bool						checkValidationLayerSupport();
	std::vector<const char*>	getRequiredExtensions();
	static std::vector<char> 	readFile(const std::string& filename);


	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);


};
