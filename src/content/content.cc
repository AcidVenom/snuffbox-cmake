#include "../content/content.h"
#include "../content/content_manager.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	Content::Content(const ContentTypes& type) : 
		type_(type),
		is_valid_(false)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	void Content::Load(const std::string& path)
	{
		SNUFF_LOG_WARNING("Attempted to load '" + path + "', but no load functionality was provided");
	}

	//---------------------------------------------------------------------------------------------------------
	void Content::Validate()
	{
		is_valid_ = true;
	}

	//---------------------------------------------------------------------------------------------------------
	void Content::Invalidate()
	{
		is_valid_ = false;
	}

	//---------------------------------------------------------------------------------------------------------
	const ContentTypes& Content::type() const
	{
		return type_;
	}

	//---------------------------------------------------------------------------------------------------------
	const bool& Content::is_valid() const
	{
		return is_valid_;
	}

	//---------------------------------------------------------------------------------------------------------
	Content::~Content()
	{

	}
}