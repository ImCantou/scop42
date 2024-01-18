#include "PipelineCreator.hpp"

VkPipeline	PipelineCreator::build_pipeline(VkDevice device, VkRenderPass pass){
	VkPipelineViewportStateCreateInfo viewportState = VulkanInitializer::viewport_state_create_info();
	viewportState.pViewports = &_viewport;
	viewportState.pScissors = &_scissor;

	//setup dummy color blending. We aren't using transparent objects yet
	//the blending is just "no blend", but we do write to the color attachment
	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.pNext = nullptr;

	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &_colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

	std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState = VulkanInitializer::dynamic_state_create_info(dynamicStates.size(), dynamicStates.data());

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = _shaderStages.data();
	pipelineInfo.pVertexInputState = &_vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &_inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &_rasterizer;
	pipelineInfo.pMultisampleState = &_multisampling;
	pipelineInfo.pDepthStencilState = &_depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = _pipelineLayout;
	pipelineInfo.renderPass = pass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	VkPipeline newPipeline;
	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline) != VK_SUCCESS) {
	    throw std::runtime_error("failed to create graphics pipeline!");
	}

	return newPipeline;
}

void		PipelineCreator::setMultisampling(VkPipelineMultisampleStateCreateInfo info) {
	this->_multisampling = info;
}

void		PipelineCreator::setColorBlendAttachment(VkPipelineColorBlendAttachmentState info) {
	this->_colorBlendAttachment = info;
}

void		PipelineCreator::setInputAssembly(VkPipelineInputAssemblyStateCreateInfo info) {
	this->_inputAssembly = info;
}

void		PipelineCreator::setVertexInput(VkPipelineVertexInputStateCreateInfo& info) {
	this->_vertexInputInfo = info;
}

void		PipelineCreator::setRasterizer(VkPipelineRasterizationStateCreateInfo info) {
	this->_rasterizer = info;
}

void		PipelineCreator::setScissor(VkRect2D scissor) {
	this->_scissor = scissor;
}

void		PipelineCreator::setViewport(VkViewport viewport) {
	this->_viewport = viewport;
}

void		PipelineCreator::addShaderStage(VkPipelineShaderStageCreateInfo shaderStage) {
	this->_shaderStages.push_back(shaderStage);
}

void		PipelineCreator::setDepthStencil(VkPipelineDepthStencilStateCreateInfo depthStencil) {
	this->_depthStencil = depthStencil;
}

void		PipelineCreator::setPipelineLayout(VkPipelineLayout layout){
	this->_pipelineLayout = layout;
}
