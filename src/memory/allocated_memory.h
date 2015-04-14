#pragma once

namespace snuffbox
{
	/**
	* @class snuffbox::AllocatedMemory
	* @brief Contains information about the number of allocations and allocated memory, also checks for leaks on destruction
	* @author Daniël Konings
	*/
	class AllocatedMemory
	{
	public:
		/// Default destructor (Checks for leaks)
		~AllocatedMemory();

		/**
		* @brief The singleton instance of this object
		* @return snuffbox::AllocatedMemory& A reference to the instance
		*/
		static AllocatedMemory& Instance();

		/**
		* @brief Constructs a pointer through given arguments
		* @param[in] args (Args&&..) The forwarded arguments to construct the pointer with
		* @return T* A pointer to the newly created object
		*/
		template<typename T, typename... Args>
		T* Construct(Args&&... args);

		/**
		* @brief Destructs a pointer
		* @param[in] ptr (T*) The pointer to destruct
		*/
		template<typename T>
		void Destruct(T* ptr);

		/// Increments the allocations by 1
		void IncreaseAllocations();

		/// Decrements the allocations by 1
		void DecreaseAllocations();

		/**
		* @brief Increments the allocated memory by a size
		* @param[in] size (const size_t& size) The allocated size of a new pointer
		*/
		void IncreaseUsedMemory(const size_t& size);

		/**
		* @brief Decrements the allocated memory by a size
		* @param[in] size (const size_t& size) The allocated size of a destructed pointer
		*/
		void DecreaseUsedMemory(const size_t& size);

    /// Checks the environment for memory leaks, if the console exists, keep it running to display the message
    void CheckForLeaks();

	private:

		/// Default constructor
		AllocatedMemory();

		unsigned int allocations_; //!< The number of allocations of this allocator
		size_t allocated_memory_; //!< The total allocated memory in bytes of this allocator
	};

	//---------------------------------------------------------------------------------------------------------
	template<typename T, typename... Args>
	inline T* AllocatedMemory::Construct(Args&&... args)
	{
		T* ptr = new T(std::forward<Args>(args)...);
		IncreaseAllocations();
		IncreaseUsedMemory(sizeof(ptr));
		return ptr;
	}

	//---------------------------------------------------------------------------------------------------------
	template<typename T>
	inline void AllocatedMemory::Destruct(T* ptr)
	{
		DecreaseAllocations();
		DecreaseUsedMemory(sizeof(ptr));

		delete ptr;

		ptr = nullptr;
	}
}