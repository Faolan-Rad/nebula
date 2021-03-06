#pragma once
//------------------------------------------------------------------------------
/**
	Implements a read/write buffer used within shaders, in Vulkan.
	
	(C) 2016-2020 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "coregraphics/shaderrwbuffer.h"
#include "vkmemory.h"
#include "ids/idallocator.h"
namespace Vulkan
{

struct VkShaderRWBufferLoadInfo
{
	VkDevice dev;
	CoreGraphics::Alloc mem;
	VkBufferCreateInfo info;

	SizeT size;
	SizeT grow;
};

struct VkShaderRWBufferRuntimeInfo
{
	VkBuffer buf;
};

struct VkShaderRWBufferMapInfo
{
	void* data;
	SizeT baseOffset;
};

typedef Ids::IdAllocator<
	VkShaderRWBufferLoadInfo,
	VkShaderRWBufferRuntimeInfo,
	VkShaderRWBufferMapInfo
> ShaderRWBufferAllocator;
extern ShaderRWBufferAllocator shaderRWBufferAllocator;

/// get vk image
const VkBuffer ShaderRWBufferGetVkBuffer(const CoreGraphics::ShaderRWBufferId id);
} // namespace Vulkan