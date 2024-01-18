#pragma once 

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>

# include <stdexcept>
# include <array>
# include "Vertex.hpp"

# include "EngineUtils.hpp"

class VulkanInitializer {

	public:
		static VkCommandPoolCreateInfo command_pool_create_info(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);
		static VkCommandBufferAllocateInfo command_buffer_allocate_info(VkCommandPool pool, uint32_t count = 1, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		static VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info(VkShaderStageFlagBits stage, VkShaderModule shaderModule);
		static VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info();
		static VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info(VkPrimitiveTopology topology);
		static VkPipelineRasterizationStateCreateInfo rasterization_state_create_info(VkPolygonMode polygonMode);
		static VkPipelineMultisampleStateCreateInfo multisampling_state_create_info();
		static VkPipelineColorBlendAttachmentState color_blend_attachment_state();
		static VkPipelineLayoutCreateInfo pipeline_layout_create_info();
		static VkPipelineViewportStateCreateInfo viewport_state_create_info();
		static VkPipelineDynamicStateCreateInfo	 dynamic_state_create_info(size_t dynamicStateCount, VkDynamicState* pDynamicStates);
		static VkPipelineDepthStencilStateCreateInfo depth_stencil_create_info();
		static VkSamplerCreateInfo sampler_create_info(VkPhysicalDeviceProperties properties);
		static VkDescriptorPoolCreateInfo descriptor_pool_create_info(uint32_t poolSize , const VkDescriptorPoolSize *pPoolSizes);
		static VkDeviceQueueCreateInfo device_queue_create_info(uint32_t family, float *pPriority);
		static VkDeviceCreateInfo device_create_info(std::vector<VkDeviceQueueCreateInfo>& queues, VkPhysicalDeviceFeatures *features, const std::vector<const char *>& extensions);
		static VkApplicationInfo application_info(const char *appName, const char *engineName);


		static VkDescriptorSetLayoutBinding descriptor_set_layout_binding(VkDescriptorType type, VkShaderStageFlagBits stageFlag);
		static VkViewport viewport_create(VkExtent2D extent);
		static VkRect2D scissor_create(VkExtent2D extent);

		static VkAttachmentDescription base_attachment_setup(VkFormat format);


};

