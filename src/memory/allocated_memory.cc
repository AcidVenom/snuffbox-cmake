#include "../memory/allocated_memory.h"
#include "../application/logging.h"

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	AllocatedMemory::AllocatedMemory() : 
		allocations_(0), 
		allocated_memory_(0)
	{
		
	}

	//-------------------------------------------------------------------------------------------
	AllocatedMemory& AllocatedMemory::Instance()
	{
		static AllocatedMemory allocated_memory;
		return allocated_memory;
	}

	//-------------------------------------------------------------------------------------------
	void AllocatedMemory::IncreaseAllocations()
	{
		++allocations_;
	}

	//-------------------------------------------------------------------------------------------
	void AllocatedMemory::DecreaseAllocations()
	{
		--allocations_;
	}

	//-------------------------------------------------------------------------------------------
	void AllocatedMemory::IncreaseUsedMemory(const size_t size)
	{
		allocated_memory_ += size;
	}

	//-------------------------------------------------------------------------------------------
	void AllocatedMemory::DecreaseUsedMemory(const size_t size)
	{
		allocated_memory_ -= size;
	}

	//-------------------------------------------------------------------------------------------
	AllocatedMemory::~AllocatedMemory()
	{
		SNUFF_XASSERT(allocations_ == 0 && allocated_memory_ == 0, "Detected a memory leak on the heap!", "AllocatedMemory::~AllocatedMemory");
		SNUFF_LOG_SUCCESS("No memory leaks detected");
		SNUFF_LOG_INFO("Shutdown succesful");
	}
}