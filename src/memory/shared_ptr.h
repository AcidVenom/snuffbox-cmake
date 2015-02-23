#pragma once

#include "../application/logging.h"
#include "../memory/allocated_memory.h"

namespace snuffbox
{
	/**
	* @class snuffbox::RefCount
	* @brief Used for snuffbox::SharedPtr<T> to keep its references
	* @author Daniël Konings
	*/
	template<typename T>
	class RefCount
	{
	public:

		/// Default constructor
		RefCount() : ref_count_(1)
		{
		
		}

		/**
		* @brief Construction through pointer
		* @param[in] ptr (T*) The pointer to construct this reference counter with
		*/
		RefCount(T* ptr) : ref_count_(1)
		{
			SNUFF_ASSERT_NOTNULL(ptr, "RefCount::RefCount(T*)");
			ptr_ = ptr;
		}

		/// Default destructor
		~RefCount()
		{
		
		}

		/// Destroys the associated pointer
		inline void Destroy()
		{
			SNUFF_ASSERT_NOTNULL(ptr_, "SharedPtr::Destroy");
			AllocatedMemory::Instance().Destruct(ptr_);
		}

		/// Increases the reference count by one
		inline void IncreaseRef(){ 
			++ref_count_;
		}

		/// Decreases the reference count by one
		inline void DecreaseRef(){ 
			if (--ref_count_ == 0)
			{
				Destroy();
			}
		}

	private:
		unsigned int ref_count_;	//!< The amount of references of this reference counter
		T* ptr_; //!< The pointer associated with this reference counter
	};

	/**
	* @class snuffbox::SharedPtr
	* @brief A garbage collecting shared pointer, so memory management won't become a worry
	* @author Daniël Konings
	*/
	template<typename T>
	class SharedPtr
	{
	public:
		/// Default constructor
		SharedPtr();

		/** 
		* @brief Construct a shared pointer containing a pointer
		* @param[in] ptr (T*) The pointer that this shared pointer will contain
		*/
		SharedPtr(T* ptr);

		/// Copy constructor
		SharedPtr(const SharedPtr<T>& other);

		/// Move constructor
		SharedPtr(SharedPtr<T>&& other);

		/// Default destructor
		~SharedPtr();

		/// Operator overload for copying
		SharedPtr& operator=(const SharedPtr& other);

		/// Operator overload for moving
		SharedPtr& operator=(SharedPtr&& other);

		/// Retrieve the contained pointer with the pointer operator
		T* operator->(void);

		/// Dereference operator
		T& operator*(void);

		/// Comparison operator with another shared pointer
		bool operator==(const SharedPtr<T>& other) const;

		/// Comparison operator with a normal pointer
		bool operator==(const T* other) const;

		/// Comparison operator with a nullptr
		bool operator==(std::nullptr_t) const;

		/// Not operator with another shared pointer
		bool operator!=(const SharedPtr<T>& other) const;

		/// Not operator with a normal pointer
		bool operator!=(const T* other) const;

		/// Not operator with a nullptr
		bool operator!=(std::nullptr_t) const;

		/**
		* @brief Swaps two SharedPtrs
		* @param[in] right (snuffbox::SharedPtr<T>&) The shared pointer to swap this one with
		*/
		void Swap(SharedPtr<T>& right);

		/// Resets the shared pointer
		void Reset();

		/**
		* @brief Resets this contained pointer to another pointer
		* @param[in] ptr (T*) The pointer to reset the contained pointer to
		* @param[in] ref (snuffbox::RefCount<T>*) The reference counter of the other contained pointer to swap this one with
		*/
		void Reset(T* ptr, RefCount<T>* ref);

		/**
		* @brief Resets this contained pointer to another pointer without modifying dataa 
		* @param[in] ptr (T*) The pointer to reset the contained pointer to
		* @param[in] ref (snuffbox::RefCount<T>*) The reference counter of the other contained pointer to swap this one with
		*/
		void ResetRaw(T* ptr, RefCount<T>* ref);

		/** 
		* @brief Retrieves the underlying pointer
		* @return (T*) The underlying pointer
		*/
		T* get();

	private:
		T* ptr_; //!< The pointer contained by this shared pointer
		RefCount<T>* ref_; //!< The references held by this pointer
	};

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	SharedPtr<T>::SharedPtr() : ref_(nullptr), ptr_(nullptr)
	{

	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	SharedPtr<T>::SharedPtr(const SharedPtr<T>& other) : ref_(nullptr), ptr_(nullptr)
	{
		Reset(other.ptr_, other.ref_);
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	SharedPtr<T>::SharedPtr(SharedPtr<T>&& other) : ptr_(static_cast<T*>(other.ptr_)), ref_(other.ref_)
	{
		other.ptr_ = nullptr;
		other.ref_ = nullptr;
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	SharedPtr<T>::SharedPtr(T* ptr)
	{
		SNUFF_ASSERT_NOTNULL(ptr, "SharedPtr::SharedPtr(T*)");
		ptr_ = ptr;
		ref_ = new RefCount<T>(ptr);
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	SharedPtr<T>::~SharedPtr()
	{
		if (ref_ != nullptr)
			ref_->DecreaseRef();
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr& other)
	{
		SharedPtr<T>(other).Swap(*this);
		return this;
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& other)
	{
		SharedPtr<T>(std::move(other)).Swap(*this);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	T* SharedPtr<T>::operator->(void)
	{
		SNUFF_ASSERT_NOTNULL(ptr_, "SharedPtr::operator->");
		return ptr_;
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	T& SharedPtr<T>::operator*(void)
	{
		SNUFF_ASSERT_NOTNULL(ptr_, "SharedPtr::operator*");
		return *ptr_;
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	bool SharedPtr<T>::operator==(const SharedPtr<T>& other) const
	{
		return other.get() == get();
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	bool SharedPtr<T>::operator==(const T* other) const
	{
		SNUFF_ASSERT_NOTNULL(other, "SharedPtr::operator==");
		return other == get();
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	bool SharedPtr<T>::operator==(std::nullptr_t) const
	{
    return ptr_ == nullptr;
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	bool SharedPtr<T>::operator!=(const SharedPtr<T>& other) const
	{
		return other.get() != get();
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	bool SharedPtr<T>::operator!=(const T* other) const
	{
		SNUFF_ASSERT_NOTNULL(other, "SharedPtr::operator!=");
		return other != get();
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	bool SharedPtr<T>::operator!=(std::nullptr_t) const
	{
		return ptr_ != nullptr
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	void SharedPtr<T>::Swap(SharedPtr<T>& right)
	{
		std::swap(ref_, right.ref_);
		std::swap(ptr_, right.ptr_);
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	void SharedPtr<T>::Reset()
	{
		if (ref_ != nullptr)
			ref_->IncreaseRef();
		ResetRaw(ptr_, ref_);
	}

	//-------------------------------------------------------------------------------------------------
	template<typename T>
	void SharedPtr<T>::Reset(T* ptr, RefCount<T>* ref)
	{
		if (ref != nullptr)
			ref->IncreaseRef();
		ResetRaw(ptr, ref);
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	void SharedPtr<T>::ResetRaw(T* ptr, RefCount<T>* ref)
	{
		if (ref_ != nullptr)
			ref_->DecreaseRef();
		ptr_ = ptr;
		ref_ = ref;
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	T* SharedPtr<T>::get()
	{
		SNUFF_ASSERT_NOTNULL(ptr_, "SharedPtr::get");
		return ptr_;
	}

}