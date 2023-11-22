#ifndef __PIPELINECREATOR_CLASS__
# define __PIPELINECREATOR_CLASS__

# include "VulkanInitializer.hpp"
# include <vector>

class PipelineCreator
{
private:
	std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
	VkPipelineVertexInputStateCreateInfo _vertexInputInfo;
	VkPipelineInputAssemblyStateCreateInfo _inputAssembly;
	VkViewport _viewport;
	VkRect2D _scissor;
	VkPipelineRasterizationStateCreateInfo _rasterizer;
	VkPipelineColorBlendAttachmentState _colorBlendAttachment;
	VkPipelineMultisampleStateCreateInfo _multisampling;
	VkPipelineDepthStencilStateCreateInfo _depthStencil;
	VkPipelineLayout _pipelineLayout;
public:
	
	VkPipeline	build_pipeline(VkDevice device, VkRenderPass pass);
	void		addShaderStage(VkPipelineShaderStageCreateInfo);
	void		setVertexInput(VkPipelineVertexInputStateCreateInfo&);
	void		setInputAssembly(VkPipelineInputAssemblyStateCreateInfo);
	void		setViewport(VkViewport);
	void		setScissor(VkRect2D);
	void		setRasterizer(VkPipelineRasterizationStateCreateInfo);
	void		setColorBlendAttachment(VkPipelineColorBlendAttachmentState);
	void		setMultisampling(VkPipelineMultisampleStateCreateInfo);
	void		setDepthStencil(VkPipelineDepthStencilStateCreateInfo);
	void		setPipelineLayout(VkPipelineLayout);
};


#endif