#ifndef __ALLOCATEDBUFFER_CLASS__
# define __ALLOCATEDBUFFER_CLASS__

#include "VulkanInitializer.hpp"

typedef struct s_AllocatedBuffer
{
	VkBuffer		buffer;
	VkDeviceMemory	memory;	
} t_AllocatedBuffer;

#endif