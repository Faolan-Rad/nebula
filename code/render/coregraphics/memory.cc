//------------------------------------------------------------------------------
//  memory.cc
//  (C) 2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "render/stdneb.h"
#include "memory.h"
#include "math/scalar.h"
namespace CoreGraphics
{

Util::Array<MemoryPool> Pools;
Threading::CriticalSection AllocationLoc;

//------------------------------------------------------------------------------
/**
*/
Alloc
MemoryPool::AllocateMemory(uint alignment, uint size)
{
	switch (this->allocMethod)
	{
	case MemoryPool_AllocConservative:
		return this->AllocateConservative(alignment, size);
	case MemoryPool_AllocLinear:
		return this->AllocateLinear(alignment, size);
	}
	return Alloc{ 0, 0, 0, UINT32_MAX, UINT32_MAX };
}

//------------------------------------------------------------------------------
/**
*/
bool 
MemoryPool::DeallocateMemory(const Alloc& alloc)
{
	switch (this->allocMethod)
	{
	case MemoryPool_AllocConservative:
		return this->DeallocConservative(alloc);
	case MemoryPool_AllocLinear:
		return this->DeallocLinear(alloc);
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
MemoryPool::Clear()
{
	for (IndexT i = 0; i < this->blocks.Size(); i++)
		this->DestroyBlock(this->blocks[i], this->mapMemory);

	this->blocks.Clear();
	this->blockRanges.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void* 
MemoryPool::GetMappedMemory(const Alloc& alloc)
{
	return (char*)this->blockMappedPointers[alloc.blockIndex] + alloc.offset;
}

//------------------------------------------------------------------------------
/**
*/
Alloc
MemoryPool::AllocateConservative(DeviceSize alignment, DeviceSize size)
{
	if (size > this->blockSize)
		n_error("Allocation of size %d would not fit into block size %d", size, this->blockSize);

	for (IndexT blockIndex = 0; blockIndex < this->blocks.Size(); blockIndex++)
	{
		Util::Array<AllocRange>& ranges = this->blockRanges[blockIndex];

		// walk through and find a hole
		DeviceSize prevOffset = 0;
		IndexT i;
		for (i = 0; i < ranges.Size(); i++)
		{
			// get the current offset
			// if the gap between the previous offset and the current
			// one fits our memory, break the loop and return the offset
			// and index, so we can insert a new range for later
			DeviceSize curOffset = ranges[i].offset;
			DeviceSize curSize = ranges[i].size;

			if (prevOffset > curOffset)
				goto next;
			else if (curOffset - prevOffset >= size)
			{
				// break, this will insert a new range after the previous but before this one
				break;
			}

		next:
			// set the new offset to be the end of the current range
			prevOffset = Math::n_align(curOffset + curSize, alignment);
		}

		// if we are going to allocate outside of the block, go to the next block
		if (prevOffset + size >= blockSize)
			continue;

		// create a new range and insert it into the list
		AllocRange range;
		range.offset = prevOffset;
		range.size = size;
		ranges.Insert(i, range);
		Alloc ret{ this->blocks[blockIndex], range.offset, range.size, this->memoryType, blockIndex };
		return ret;
	}

	// no gap big enough could be found, create new block
	n_assert(this->size + this->blockSize < this->maxSize);
	this->size += this->blockSize;

	SizeT newBlockIndex = this->blocks.Size();
	this->blockMappedPointers.Append(nullptr);
	DeviceMemory mem = this->CreateBlock(this->mapMemory, &this->blockMappedPointers[newBlockIndex]);
	this->blocks.Append(mem);
	this->blockRanges.Append({ AllocRange{0, size} });
	Alloc ret{ this->blocks.Back(), DeviceSize(0), size, this->memoryType, newBlockIndex };
	return ret;
}

//------------------------------------------------------------------------------
/**
*/
bool 
MemoryPool::DeallocConservative(const Alloc& alloc)
{
	Util::Array<AllocRange>& ranges = this->blockRanges[alloc.blockIndex];
	for (IndexT i = 0; i < ranges.Size(); i++)
	{
		if (ranges[i].offset == alloc.offset)
		{
			ranges.EraseIndex(i);

			// if the range is empty, dealloc the memory block
			if (ranges.IsEmpty())
			{
				// deallocate the memory and remove this index
				this->DestroyBlock(this->blocks[alloc.blockIndex], this->mapMemory);
				this->blocks.EraseIndex(alloc.blockIndex);
				this->blockRanges.EraseIndex(alloc.blockIndex);
				this->blockMappedPointers.EraseIndex(alloc.blockIndex);
			}
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
Alloc
MemoryPool::AllocateLinear(DeviceSize alignment, DeviceSize size)
{
	if (size > this->blockSize)
		n_error("Allocation of size %d would not fit into block size %d", size, this->blockSize);

	for (IndexT blockIndex = 0; blockIndex < this->blocks.Size(); blockIndex++)
	{
		Util::Array<AllocRange>& ranges = this->blockRanges[blockIndex];
		if (ranges.Size() > 0)
		{
			const AllocRange& lastRange = ranges.Back();
			AllocRange range;
			range.offset = Math::n_align(lastRange.offset + lastRange.size, alignment);
			range.size = size;

			// if we are over-subscribing a block, go to the next block
			if (range.offset + range.size >= this->blockSize)
				continue;

			ranges.Append(range);
			Alloc ret{ this->blocks[blockIndex], range.offset, range.size, this->memoryType, blockIndex };
			return ret;
		}
		else
		{
			AllocRange range;
			range.offset = 0;
			range.size = size;

			// no need to safe-proof the code here since we know at the first condition if we have space to allocate
			ranges.Append(range);

			Alloc ret{ this->blocks[blockIndex], range.offset, range.size, this->memoryType, blockIndex };
			return ret;
		};
	}

	// no range could be found, so allocate new block
	n_assert(this->size + this->blockSize < this->maxSize);
	this->size += this->blockSize;

	SizeT newBlockIndex = this->blocks.Size();
	this->blockMappedPointers.Append(nullptr);
	DeviceMemory mem = this->CreateBlock(this->mapMemory, &this->blockMappedPointers[newBlockIndex]);
	this->blocks.Append(mem);
	this->blockRanges.Append({ AllocRange{0, size} });
	Alloc ret{ this->blocks.Back(), DeviceSize(0), size, this->memoryType, newBlockIndex };
	return ret;
}

//------------------------------------------------------------------------------
/**
*/
bool 
MemoryPool::DeallocLinear(const Alloc& alloc)
{
	Util::Array<AllocRange>& ranges = this->blockRanges[alloc.blockIndex];
	for (IndexT i = 0; i < ranges.Size(); i++)
	{
		if (ranges[i].offset == alloc.offset)
		{
			ranges.EraseIndex(i);

			// if the range is empty, dealloc the memory block
			if (ranges.IsEmpty())
			{
				// deallocate the memory and remove this index
				this->DestroyBlock(this->blocks[alloc.blockIndex], this->mapMemory);
				this->blocks.EraseIndex(alloc.blockIndex);
				this->blockRanges.EraseIndex(alloc.blockIndex);
				this->blockMappedPointers.EraseIndex(alloc.blockIndex);
			}
			return true;
		}
	}
	return false;
}

} // namespace CoreGraphics
