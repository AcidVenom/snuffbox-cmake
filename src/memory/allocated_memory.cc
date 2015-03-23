#include "../memory/allocated_memory.h"
#include "../application/logging.h"

#ifdef SNUFF_BUILD_CONSOLE
  #include "../console/console.h"
#endif

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

  void AllocatedMemory::CheckForLeaks()
  {
    SNUFF_XASSERT(allocations_ == 0 && allocated_memory_ == 0, "Detected a memory leak on the heap, allocations: " + std::to_string(allocations_) + ", allocated: " + std::to_string(allocated_memory_) + " bytes", "AllocatedMemory::CheckForLeaks");
    SNUFF_LOG_SUCCESS("No memory leaks detected");
		SNUFF_LOG_SUCCESS("Shutdown succesful");

#ifdef SNUFF_BUILD_CONSOLE
    Console* console = Console::Instance();
    SNUFF_ASSERT_NOTNULL(console, "AllocatedMemory::CheckForLeaks::console");
    
    while (console->enabled() && console->IsVisible())
    {
      qApp->exec();
    }

    delete console;
    console = nullptr;
#endif
  }

	//-------------------------------------------------------------------------------------------
	AllocatedMemory::~AllocatedMemory()
	{
		SNUFF_LOG_INFO("Checking for leaks");
    CheckForLeaks();
	}
}