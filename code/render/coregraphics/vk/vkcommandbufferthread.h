#pragma once
//------------------------------------------------------------------------------
/**
	This thread records commands to a Vulkan Command Buffer in its own thread.
	
	(C) 2016-2020 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <vulkan/vulkan.h>
#include "coregraphics/drawthread.h"
#include "coregraphics/config.h"
#include "threading/thread.h"
#include "threading/safequeue.h"
#include "coregraphics/primitivegroup.h"
#include "debug/debugtimer.h"
#include "math/rectangle.h"
#include "coregraphics/indextype.h"

namespace Vulkan
{
class VkCommandBufferThread : public CoreGraphics::DrawThread
{
	__DeclareClass(VkCommandBufferThread);

public:

	enum VkCommandType
	{
		BeginCommand,
		ResetCommands,
		EndCommand,
		GraphicsPipeline,
		ComputePipeline,
		InputAssemblyVertex,
		InputAssemblyIndex,
		Draw,
		Dispatch,
		BindDescriptors,
		PushRange,
		Viewport,
		ViewportArray,
		ScissorRect,
		ScissorRectArray,
		UpdateBuffer,
		SetEvent,					// sets event to flagged
		ResetEvent,					// resets event to unflagged
		WaitForEvent,
		Barrier,
		Sync,
		BeginMarker,
		EndMarker,
		InsertMarker
	};


	struct VkCommand
	{
		union
		{
			struct // Pipeline bind
			{
				VkPipeline pipeline;
				VkPipelineLayout layout;
#if NEBULA_GRAPHICS_DEBUG
				const char* name;
#endif
			} pipe;

			struct // BeginCmd
			{
				VkCommandBufferBeginInfo info;
				VkCommandBufferInheritanceInfo inheritInfo;
				VkCommandBuffer buf;
			} bgCmd;

			struct // VBO
			{
				VkBuffer buffer;
				IndexT index;
				VkDeviceSize offset;
			} vbo;

			struct // IBO
			{
				VkBuffer buffer;
				VkDeviceSize offset;
				VkIndexType indexType;
			} ibo;

			struct // Draw
			{
				uint32_t baseIndex;
				uint32_t baseVertex;
				uint32_t numIndices;
				uint32_t numVerts;
				uint32_t baseInstance;
				uint32_t numInstances;
			} draw;

			struct // Dispatch
			{
				uint32_t numGroupsX;
				uint32_t numGroupsY;
				uint32_t numGroupsZ;
			} dispatch;

			struct // Descriptors
			{
				VkPipelineBindPoint type;
				uint32_t baseSet;
				uint32_t numSets;
				const VkDescriptorSet* sets;
				uint32_t numOffsets;
				const uint32_t* offsets;
			} descriptor;

			struct // UpdateBuffer
			{
				bool deleteWhenDone;
				VkBuffer buf;
				VkDeviceSize offset;
				VkDeviceSize size;
				uint32_t* data;
			} updBuffer;

			struct // PushConstants
			{
				VkShaderStageFlags stages;
				VkPipelineLayout layout;
				uint32_t offset;
				uint32_t size;
				void* data;
			} pushranges;

			struct // Viewport
			{
				VkViewport vp;
				uint32_t index;
			} viewport;

			struct // ViewportArray
			{
				VkViewport vps[8];
				uint32_t first;
				uint32_t num;
			} viewportArray;

			struct // ScissorRect
			{
				VkRect2D sc;
				uint32_t index;
			} scissorRect;

			struct // ScissorRectArray
			{
				VkRect2D scs[8];
				uint32_t first;
				uint32_t num;
			} scissorRectArray;

			struct // SetEvent
			{
				VkEvent event;
				VkPipelineStageFlags stages;
			} setEvent;

			struct // ResetEvent
			{
				VkEvent event;
				VkPipelineStageFlags stages;
			} resetEvent;

			struct // WaitForEvents
			{
				VkEvent* events;
				uint32_t numEvents;
				VkPipelineStageFlags signalingStage;
				VkPipelineStageFlags waitingStage;
				uint32_t memoryBarrierCount;
				VkMemoryBarrier* memoryBarriers;
				uint32_t bufferBarrierCount;
				VkBufferMemoryBarrier* bufferBarriers;
				uint32_t imageBarrierCount;
				VkImageMemoryBarrier* imageBarriers;
			} waitEvent;

			struct // Barrier
			{
				VkPipelineStageFlags srcMask;
				VkPipelineStageFlags dstMask;
				VkDependencyFlags dep;
				uint32_t memoryBarrierCount;
				VkMemoryBarrier* memoryBarriers;
				uint32_t bufferBarrierCount;
				VkBufferMemoryBarrier* bufferBarriers;
				uint32_t imageBarrierCount;
				VkImageMemoryBarrier* imageBarriers;
			} barrier;

			struct // marker
			{
				const char* text;
				float values[4];
			} marker;

		};

		VkCommandType type;

	};

	/// constructor
	VkCommandBufferThread();
	/// destructor
	virtual ~VkCommandBufferThread();

	/// this method runs in the thread context
	void DoWork() override;
private:
	VkCommandBuffer vkCommandBuffer;
	VkPipelineLayout vkPipelineLayout;

#if NEBULA_ENABLE_PROFILING
	_declare_timer(debugTimer);
#endif
};

} // namespace Vulkan