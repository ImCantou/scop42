
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "ParserObj.hpp"

#include "VulkanInitializer.hpp"

#include "PipelineCreator.hpp"

#include "DeletionQueue.hpp"

#include "AllocatedBuffer.hpp"

#include "Camera3D.hpp"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
const int MAX_FRAMES_IN_FLIGHT = 2;

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR 		capabilities;
    std::vector<VkSurfaceFormatKHR>	formats;
    std::vector<VkPresentModeKHR>	presentModes;
};

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

class VulkanEngine {
public:
	VulkanEngine(std::string path): pathToFile(path) {}

    void run() {
        initWindow();
	    initVulkan();
	    mainLoop();
	    cleanup();
    }

private:
	//Window basic components
	GLFWwindow* 					window;
	VkInstance 						instance;
	
	//Validation Layers manager (print message when vulkan not used properly)
	VkDebugUtilsMessengerEXT		debugMessenger;

	//Physical and logical device (GPU)
	VkPhysicalDevice 				physicalDevice = VK_NULL_HANDLE;
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

	// DeletionQueue For Vulkan Components on cleanup
	DeletionQueue					_mainDeletionQueue;

	//Basic variables
	uint32_t	currentFrame = 0;
	bool		isInitialized = false;
	bool		wireframeMode = false;
	bool		rotationMode  = false;
	bool		framebufferResized = false;
	Camera3D	camera;
	glm::vec2	mousePos;
	glm::mat4	modelRotation = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::vec2	rotationOffset;



    void initWindow() {
	    glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		// glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		
		window = glfwCreateWindow(WIDTH, HEIGHT, "Scop 42", nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
		glfwSetKeyCallback(window, key_callback);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);

    }

	static void	scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		auto app = reinterpret_cast<VulkanEngine*>(glfwGetWindowUserPointer(window));
		app->camera.zoom(yoffset);
	}

	static void	mouse_callback(GLFWwindow* window, double xpos, double ypos) {
		auto app = reinterpret_cast<VulkanEngine*>(glfwGetWindowUserPointer(window));
		static bool first = true;
		if (first) {
			app->mousePos.x = xpos;
			app->mousePos.y = ypos;
			first = false;
		}

		float xoffset = (xpos - app->mousePos.x) * 0.1;
		float yoffset = (app->mousePos.y - ypos) * 0.1;

		// if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		if (app->rotationMode) {
			app->rotationOffset.x += xoffset;
			app->rotationOffset.y += yoffset;
			app->modelRotation = app->modelRotation + glm::rotate(glm::mat4(1.0f), yoffset * glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f))
				 + glm::rotate(glm::mat4(1.0f), xoffset * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		}
		else {
			
			app->camera.rotate(xoffset, yoffset);
		}
		app->mousePos.x = xpos;
		app->mousePos.y = ypos;
	}

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{	
		auto app = reinterpret_cast<VulkanEngine*>(glfwGetWindowUserPointer(window));
		std::cout << key << std::endl;
	    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
			app->wireframeMode = !app->wireframeMode;
		}
		if (key == GLFW_KEY_R && action == GLFW_PRESS) {
			app->rotationMode = !app->rotationMode;
		}
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, 1);
		}
	}

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<VulkanEngine*>(glfwGetWindowUserPointer(window));
    	app->framebufferResized = true;
	}


    void initVulkan() {
		createInstance();
	    setupDebugMessenger();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
		createImageViews();
		createRenderPass();
		createDescriptorSetLayout();
		createGraphicsPipeline();
		createCommandPool();
		createDepthResources();
		createFramebuffers();
		createTextureImage();
		createTextureImageView();
		createTextureSampler();
		loadModel();
		createVertexBuffer();
		createIndexBuffer();
		createUniformBuffers();
		createDescriptorPool();
		createDescriptorSets();
		createCommandBuffer();
		createSyncObjects();

		isInitialized = true;
	}

	void	loadModel() {
		this->model = ParserObj::parseFile(pathToFile);
	}

	void	createDepthResources() {
		VkFormat	depthFormat = findDepthFormat();

		createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
		depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

		transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	}

	VkFormat	findDepthFormat() {
		return findSupportedFormat(
			{VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	VkFormat	findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
				return format;
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
				return format;
		}

		throw std::runtime_error("failed to find supported format!");
	}

	bool hasStencilComponent(VkFormat format) {
    	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	void createTextureSampler() {
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;

		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		samplerInfo.anisotropyEnable = VK_TRUE;
		
		VkPhysicalDeviceProperties properties = {};
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);

		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if(vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}

		_mainDeletionQueue.add([=, this]() {
			vkDestroySampler(device, textureSampler, nullptr);
		});
	}

	void createTextureImageView() {
		textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

		_mainDeletionQueue.add([=, this]() {
			vkDestroyImageView(device, textureImageView, nullptr);
		});
	}

	VkImageView	createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = image;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = format;
		
		createInfo.subresourceRange.aspectMask = aspectFlags;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		
		VkImageView	imageView;
		if (vkCreateImageView(device, &createInfo, nullptr, &imageView) != VK_SUCCESS) {
    		throw std::runtime_error("failed to create image view!");
		}

		return imageView;
	}

	void	createTextureImage() {
		// read Texture image to pixels using STBI library
		int texWidth, texHeight, texChannels;
		stbi_uc*	pixels = stbi_load("/mnt/nfs/homes/qcherel/Documents/OpenGL/scop42/Textures/banana.jpeg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize	imgSize = texWidth * texHeight * STBI_rgb_alpha;

		if (!pixels) {
			throw std::runtime_error("failed to load texture image!");
		}

		VkBuffer 		stagingBuffer;
		VkDeviceMemory	stagingBufferMemory;

		// Create buffer to store the image
		createBuffer(imgSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		// Transfer pixels data to the buffer
		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, imgSize, 0, &data);
			memcpy(data, pixels, static_cast<size_t>(imgSize));
		vkUnmapMemory(device, stagingBufferMemory);

		// pixels is no longer needed
		stbi_image_free(pixels);

		createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

		transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
		transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);

		_mainDeletionQueue.add([=, this]() {
			vkDestroyImage(device, textureImage, nullptr);
			vkFreeMemory(device, textureImageMemory, nullptr);
		});
	}



	void	createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags props, VkImage& image, VkDeviceMemory& imageMemory) 
	{
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.depth = 1;
		imageInfo.extent.height = height;
		imageInfo.extent.width = width;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0;

		if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS)
			throw std::runtime_error("failed to create image!");

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, props);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		    throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(device, image, imageMemory, 0);
	}

	void	transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		
		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (hasStencilComponent(format))
				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
		else
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		
		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.srcAccessMask = 0;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} 
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    		barrier.srcAccessMask = 0;
    		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

	    endSingleTimeCommands(commandBuffer);
	}

	void	copyBufferToImage(VkBuffer buffer, VkImage image, u_int32_t width, uint32_t height) {
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageOffset = {0,0,0};
		region.imageExtent = { width, height, 1};

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		endSingleTimeCommands(commandBuffer);
	}

	void createDescriptorSets() {
		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
		
		if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
    		throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    		VkDescriptorBufferInfo bufferInfo = {};
    		bufferInfo.buffer = uniformABuffers[i].buffer;
    		bufferInfo.offset = 0;
    		bufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorImageInfo	imageInfo = {};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = textureImageView;
			imageInfo.sampler = textureSampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = descriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;
			descriptorWrites[0].pImageInfo = nullptr;
			descriptorWrites[0].pTexelBufferView = nullptr;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = descriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pBufferInfo = nullptr;
			descriptorWrites[1].pImageInfo = &imageInfo;
			descriptorWrites[1].pTexelBufferView = nullptr;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0 ,nullptr);
		}
	}

	void createDescriptorPool() {
		std::array<VkDescriptorPoolSize, 2> poolSizes = {};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);


		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		poolInfo.flags = 0;

		if(vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}

		_mainDeletionQueue.add([=, this]() {
	    	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
		});
	}

	void createUniformBuffers() {
		VkDeviceSize	bufferSize = sizeof(UniformBufferObject);
	
		uniformABuffers.resize(MAX_FRAMES_IN_FLIGHT);
    	uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformABuffers[i].buffer, uniformABuffers[i].memory);

			vkMapMemory(device, uniformABuffers[i].memory, 0, bufferSize, 0, &uniformBuffersMapped[i]);
		}

		_mainDeletionQueue.add([=, this]() {
			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
				vkDestroyBuffer(device, uniformABuffers[i].buffer, nullptr);
				vkFreeMemory(device, uniformABuffers[i].memory, nullptr);
			}
		});
	}

	void createDescriptorSetLayout() {
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding	samplerLayoutBinding = {};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}

		_mainDeletionQueue.add([=, this]() {
			vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
		});

	} 

	void createIndexBuffer() {
		VkDeviceSize bufferSize = sizeof(model.getIndices()[0]) * model.getIndices().size();

		t_AllocatedBuffer	stagingABuffer;
    	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingABuffer.buffer, stagingABuffer.memory);

    	void* data;
    	vkMapMemory(device, stagingABuffer.memory, 0, bufferSize, 0, &data);
    	memcpy(data, model.getIndices().data(), (size_t) bufferSize);
    	vkUnmapMemory(device, stagingABuffer.memory);

    	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexABuffer.buffer, indexABuffer.memory);

    	copyBuffer(stagingABuffer.buffer, indexABuffer.buffer, bufferSize);

    	vkDestroyBuffer(device, stagingABuffer.buffer, nullptr);
    	vkFreeMemory(device, stagingABuffer.memory, nullptr);

		_mainDeletionQueue.add([=, this]() {
			vkDestroyBuffer(device, indexABuffer.buffer, nullptr);
			vkFreeMemory(device, indexABuffer.memory, nullptr);
		});
	}

	void createVertexBuffer() {
		VkDeviceSize bufferSize = sizeof(model.getVertices()[0]) * model.getVertices().size();
		
		t_AllocatedBuffer	stagingABuffer;

		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingABuffer.buffer, stagingABuffer.memory);

		void*	data;
		vkMapMemory(device, stagingABuffer.memory, 0, bufferSize, 0, &data);
			memcpy(data, model.getVertices().data(), (size_t) bufferSize);
		vkUnmapMemory(device, stagingABuffer.memory);

		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexABuffer.buffer, vertexABuffer.memory);

		copyBuffer(stagingABuffer.buffer, vertexABuffer.buffer, bufferSize);

		vkDestroyBuffer(device, stagingABuffer.buffer, nullptr);
		vkFreeMemory(device, stagingABuffer.memory, nullptr);

		_mainDeletionQueue.add([=, this]() {
			vkDestroyBuffer(device, vertexABuffer.buffer, nullptr);
			vkFreeMemory(device, vertexABuffer.memory, nullptr);
		});
	}

	void	copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size) {
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);

		endSingleTimeCommands(commandBuffer);
	}

	VkCommandBuffer	beginSingleTimeCommands() {
		VkCommandBufferAllocateInfo allocInfo = VulkanInitializer::command_buffer_allocate_info(commandPool);

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    	vkEndCommandBuffer(commandBuffer);

    	VkSubmitInfo submitInfo = {};
    	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    	submitInfo.commandBufferCount = 1;
    	submitInfo.pCommandBuffers = &commandBuffer;

    	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    	vkQueueWaitIdle(graphicsQueue);

    	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	}

	uint32_t	findMemoryType(uint32_t typefilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
		
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
			if ((typefilter & ( 1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	void	createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory &bufferMemory) {
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create buffer!");
		}
		
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		vkBindBufferMemory(device, buffer, bufferMemory, 0);
	}

	void createSyncObjects() {
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo	semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo 		fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS
    			|| vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS
    			|| vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
    			throw std::runtime_error("failed to create semaphores!");
			}
		}

		_mainDeletionQueue.add([=, this]() {
			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
        		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
        		vkDestroyFence(device, inFlightFences[i], nullptr);
    		}
		});

	}

	void createCommandBuffer() {
		commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo = VulkanInitializer::command_buffer_allocate_info(commandPool, commandBuffers.size());

		if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		beginInfo.pInheritanceInfo = nullptr;

		if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = swapChainExtent;

		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
		clearValues[1].depthStencil = {1.0f, 0};
		
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		if (wireframeMode)
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineWireframe);
		else
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

		VkBuffer		vertexBuffers[] = {vertexABuffer.buffer};
		VkDeviceSize	offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(commandBuffer, indexABuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapChainExtent.width);
		viewport.height = static_cast<float>(swapChainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset = {0,0};
		scissor.extent = swapChainExtent;

		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	
		// vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(model.getIndices().size()), 1, 0, 0, 0);

		vkCmdEndRenderPass(commandBuffer);

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    		throw std::runtime_error("failed to record command buffer!");
		}
	}

	void createCommandPool() {
		QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

		VkCommandPoolCreateInfo poolInfo = VulkanInitializer::command_pool_create_info(queueFamilyIndices.graphicsFamily.value(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	
		if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
    		throw std::runtime_error("failed to create command pool!");
		}

		_mainDeletionQueue.add([=, this]() {
			vkDestroyCommandPool(device, commandPool, nullptr);
		});
	}

	void createFramebuffers() {
		swapChainFramebuffers.resize(swapChainImageViews.size());
		
		VkFramebufferCreateInfo framebufferInfo = {};
    	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    	framebufferInfo.renderPass = renderPass;
    	framebufferInfo.attachmentCount = 2;
    	
    	framebufferInfo.width = swapChainExtent.width;
    	framebufferInfo.height = swapChainExtent.height;
    	framebufferInfo.layers = 1;

		for (size_t i = 0; i < swapChainImageViews.size(); ++i) {
			std::array<VkImageView, 2> attachments = {swapChainImageViews[i], depthImageView};

			framebufferInfo.pAttachments = attachments.data();
    		
    		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
    		    throw std::runtime_error("failed to create framebuffer!");
    		}
		}
	}

	void createRenderPass() {
    	VkAttachmentDescription colorAttachment = {};
    	colorAttachment.format = swapChainImageFormat;
    	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		
		// clear data to constant at start
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		// store rendered content in memory
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = findDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT; //wait for last stage of reading image before we can access it
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT; //wait for last stage of reading image before we can access it
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; //wait for write

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		


		if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		    throw std::runtime_error("failed to create render pass!");
		}

		_mainDeletionQueue.add([=, this]() {
			vkDestroyRenderPass(device, renderPass, nullptr);
		});
	}

	void createGraphicsPipeline() {
    	std::vector<char> vertShaderCode = readFile("compiled_shaders/vert.spv");
    	std::vector<char> fragShaderCode = readFile("compiled_shaders/frag.spv");

		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = VulkanInitializer::pipeline_layout_create_info();
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

		_mainDeletionQueue.add([=, this]() {
			vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		});


		VkVertexInputBindingDescription 					bindingDescription = Vertex::getBindingDescription();
		std::array<VkVertexInputAttributeDescription, 3UL>  attributeDescriptions = Vertex::getAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputStateInfo = VulkanInitializer::vertex_input_state_create_info();
		vertexInputStateInfo.vertexBindingDescriptionCount = 1;
    	vertexInputStateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputStateInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputStateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		PipelineCreator	pipelineCreator;

		pipelineCreator.addShaderStage(VulkanInitializer::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, vertShaderModule));
		pipelineCreator.addShaderStage(VulkanInitializer::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderModule));
		pipelineCreator.setInputAssembly(VulkanInitializer::input_assembly_create_info(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST));
		pipelineCreator.setRasterizer(VulkanInitializer::rasterization_state_create_info(VK_POLYGON_MODE_FILL));
		pipelineCreator.setMultisampling(VulkanInitializer::multisampling_state_create_info());
		pipelineCreator.setColorBlendAttachment(VulkanInitializer::color_blend_attachment_state());
		pipelineCreator.setDepthStencil(VulkanInitializer::depth_stencil_create_info());
        pipelineCreator.setPipelineLayout(pipelineLayout);
		pipelineCreator.setVertexInput(vertexInputStateInfo);

		graphicsPipeline = pipelineCreator.build_pipeline(device, renderPass);

		_mainDeletionQueue.add([=, this]() {
			vkDestroyPipeline(device, graphicsPipeline, nullptr);
		});

		pipelineCreator.setRasterizer(VulkanInitializer::rasterization_state_create_info(VK_POLYGON_MODE_LINE));

		graphicsPipelineWireframe = pipelineCreator.build_pipeline(device, renderPass);

		_mainDeletionQueue.add([=, this]() {
			vkDestroyPipeline(device, graphicsPipelineWireframe, nullptr);
		});

		vkDestroyShaderModule(device, fragShaderModule, nullptr);
    	vkDestroyShaderModule(device, vertShaderModule, nullptr);

	}

	VkShaderModule	createShaderModule(const std::vector<char>& code) {
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}
		
		return shaderModule;
	}

	void createImageViews() {
		swapChainImageViews.resize(swapChainImages.size());
		for (size_t i = 0; i < swapChainImages.size(); ++i) {
			swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
		}
	}

	void createSwapChain() {
    	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
	
    	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
	
		// defining size of the swap chain
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
    		imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

		if (indices.graphicsFamily != indices.presentFamily) {
		    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		    createInfo.queueFamilyIndexCount = 2;
		    createInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
		    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		    createInfo.queueFamilyIndexCount = 0; // Optional
		    createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		// specify if you want to transform image before displaying it (rot90, flip, etc.)
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
    		throw std::runtime_error("failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
	
		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}

	void createSurface() {
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        	throw std::runtime_error("failed to create window surface!");
    	}
	}

	void createLogicalDevice() {
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

		VkPhysicalDeviceFeatures deviceFeatures = {};
		vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
    		throw std::runtime_error("failed to create logical device!");
		}

		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

	}


	void pickPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	
		if (deviceCount == 0) {
    		throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (const auto& device : devices) {
			if (isDeviceSuitable(device)) {
				physicalDevice = device;
				break;
			}
		}

		if (physicalDevice == VK_NULL_HANDLE)
			throw std::runtime_error("failed to find a suitable GPU!");
	}

	bool isDeviceSuitable(VkPhysicalDevice device) {

		// Getting if there is a graphical Queue
		QueueFamilyIndices indices = findQueueFamilies(device);
		
		// Getting supported Vulkan versions type and name and other basic properties of the device
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		// Getting supported features (shader textureCompression 64b Floats multiVP rendering)
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		//  std::cout << "name : " << deviceProperties.deviceName << std::endl;
		//  std::cout << "MaxOutputVertices : " << deviceProperties.limits.maxGeometryOutputVertices << std::endl;
		//  std::cout << "maxImageDimension2D : " << deviceProperties.limits.maxImageDimension2D << std::endl;
		//  std::cout << "maxImageDimension3D : " << deviceProperties.limits.maxImageDimension3D << std::endl;
		//  std::cout << "maxComputeSharedMemorySize : " << deviceProperties.limits.maxComputeSharedMemorySize << std::endl;
		//  std::cout << "geometryShader : " << deviceFeatures.geometryShader << std::endl;
		//  std::cout << std::endl;

		bool	extensionsSupported = checkDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionsSupported) {
		    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		    swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader && indices.isComplete() && extensionsSupported && swapChainAdequate && deviceFeatures.samplerAnisotropy; 
	}

	bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const VkExtensionProperties& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

			 VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport) {
                indices.presentFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }

            ++i;
        }

        return indices;
    }

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
    	SwapChainSupportDetails details;

		// Querying Basic surface capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);


		// Querying supported surface formats
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0) {
		    details.formats.resize(formatCount);
		    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}


		// Querying supported presentation modes
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
		    details.presentModes.resize(presentModeCount);
		    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

    	return details;
	}

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
    		// Checking if format 32bits SRGB color is available
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        		return availableFormat;
    		}
		}
		// might rank formats and return best if preferred is not available


		//return first format if preferred isnt available
		return availableFormats[0];
	}

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
        // (swap chain is like VK_PRESENT_MODE_FIFO_KHR but refresh on queue full)
		// result in faster rendering but harsher on GPU 
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
        	    return availablePresentMode;
        	}
    	}
	
		// Guaranteed to be present
		// (swap chain is a queue, display front of queue on refresh and insert rendered imgs to the back)
		// similar to VSync to avoid tearing
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    	    return capabilities.currentExtent;
    	} else {
    	    int width, height;
    	    glfwGetFramebufferSize(window, &width, &height);

    	    VkExtent2D actualExtent = {
    	        static_cast<uint32_t>(width),
    	        static_cast<uint32_t>(height)
    	    };

    	    actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    	    actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    	    return actualExtent;
    	}
	}

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	    createInfo = {};
	    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	    createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr;

	}

	void setupDebugMessenger() {
	    if (!enableValidationLayers) return;
		
		VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
		populateDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
	}

    void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
        	glfwPollEvents();
			drawFrame();
    	}

		vkDeviceWaitIdle(device);
    }

	void drawFrame() {
		vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		// Asks GPU to provide an imageIndex (send imageAvailableSemaphore when finished)
		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		vkResetFences(device, 1, &inFlightFences[currentFrame]);

		vkResetCommandBuffer(commandBuffers[currentFrame], 0);

		recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

		updateUniformBuffer(currentFrame);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

		VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = {swapChain};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		result = vkQueuePresentKHR(presentQueue, &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
    		framebufferResized = false;
			recreateSwapChain();
		} else if (result != VK_SUCCESS) {
    		throw std::runtime_error("failed to present swap chain image!");
		}

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void updateUniformBuffer(uint32_t currentImage) {
		float cameraSpeed = 0.006f;
    	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    	    camera.move(CAM_TRANSLATE_FORWARDS, cameraSpeed);
    	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    	    camera.move(CAM_TRANSLATE_BACKWARDS, cameraSpeed);
    	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    	    camera.move(CAM_TRANSLATE_LEFT, cameraSpeed);
    	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    	    camera.move(CAM_TRANSLATE_RIGHT, cameraSpeed);

    	static auto startTime = std::chrono::high_resolution_clock::now();

    	auto currentTime = std::chrono::high_resolution_clock::now();
    	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	
		UniformBufferObject ubo = {};
		ubo.model = glm::rotate(glm::mat4(1.0f), rotationOffset.x * glm::radians(10.0f), glm::vec3( 0.0f, 0.0f, 1.0f)) * glm::rotate(glm::mat4(1.0f), rotationOffset.y * glm::radians(10.0f), glm::vec3( 1.0f, 0.0f, 0.0f)) ;

		ubo.view = camera.lookAt();

		ubo.proj = glm::perspective(glm::radians(camera.getFov()), swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 400.0f);

		ubo.proj[1][1] *= -1;

		memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
	}

    void cleanup() {
		if (isInitialized) {

			vkWaitForFences(device, 1, &inFlightFences[currentFrame], true, UINT64_MAX);

			cleanupSwapChain();

			_mainDeletionQueue.execute();

			vkDestroyDevice(device, nullptr);

			if (enableValidationLayers) {
    		    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    		}

        	vkDestroySurfaceKHR(instance, surface, nullptr);
			vkDestroyInstance(instance, nullptr);

    		glfwDestroyWindow(window);
    		glfwTerminate();
		}
	}

	void cleanupSwapChain() {
		for (VkFramebuffer framebuffer : swapChainFramebuffers) {
        	vkDestroyFramebuffer(device, framebuffer, nullptr);
    	}

    	for (VkImageView imageView : swapChainImageViews) {
        	vkDestroyImageView(device, imageView, nullptr);
    	}

		vkDestroyImageView(device, depthImageView, nullptr);
    	vkDestroyImage(device, depthImage, nullptr);
    	vkFreeMemory(device, depthImageMemory, nullptr);

    	vkDestroySwapchainKHR(device, swapChain, nullptr);
	}

	void	recreateSwapChain() {
		int		width = 0;
		int		height = 0;
    	
		glfwGetFramebufferSize(window, &width, &height);
    	while (width == 0 || height == 0) {
        	glfwGetFramebufferSize(window, &width, &height);
        	glfwWaitEvents();
    	}

		vkDeviceWaitIdle(device);

		cleanupSwapChain();
		
		createSwapChain();
		createImageViews();
		createDepthResources();
		createFramebuffers();
	}

	void createInstance() {
		if (enableValidationLayers && !checkValidationLayerSupport()) {
    	    throw std::runtime_error("validation layers requested, but not available!");
	    }

    	VkApplicationInfo appInfo = {};
    	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    	appInfo.pApplicationName = "Scop 42";
    	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    	appInfo.pEngineName = "Vulkan Engine";
    	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    	appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
        if (enableValidationLayers) {
		    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		    createInfo.ppEnabledLayerNames = validationLayers.data();

			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
		} else {
		    createInfo.enabledLayerCount = 0;

			createInfo.pNext = nullptr;
		}

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
	}

	bool checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

	std::vector<const char*> getRequiredExtensions() {
    	uint32_t 				glfwExtensionCount = 0;
    	const char**			glfwExtensions;
    	
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    	if (enableValidationLayers) {
    	    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    	}

    	return extensions;
	}


	static std::vector<char> readFile(const std::string& filename) {
    	std::ifstream file(filename, std::ios::ate | std::ios::binary);

    	if (!file.is_open()) {
    	    throw std::runtime_error("failed to open file!");
    	}

		size_t fileSize = (size_t) file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}


	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    	VkDebugUtilsMessageTypeFlagsEXT messageType,
    	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    	void* pUserData) {

		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		}

    	return VK_FALSE;
	}
};





int main(int ac, char **av) {
	if (ac != 2)
	{
		std::cout << "usage: ./VulkanTest [path to .obj file]." << std::endl;
		return (0);
	}
    VulkanEngine app(av[1]);

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}