#pragma once

#include "VulkanInitializer.hpp"

typedef struct s_AllocatedBuffer
{
	VkBuffer		buffer;
	VkDeviceMemory	memory;	
} t_AllocatedBuffer;
