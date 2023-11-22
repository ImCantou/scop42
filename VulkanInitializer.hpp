#ifndef __VULKANINITIALIZER_CLASS__
# define __VULKANINITIALIZER_CLASS__

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>

# include <stdexcept>
# include <array>
# include "Vertex.hpp"

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
};

#endif