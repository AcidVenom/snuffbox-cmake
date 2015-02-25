#include "../content/content.h"
#include "../content/content_manager.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	Content::Content(const ContentTypes& type)
		: type_(type)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	void Content::Load(const std::string& path)
	{
		SNUFF_LOG_WARNING("Attempted to load '" + path + "', but no load functionality was provided");
	}

	//---------------------------------------------------------------------------------------------------------
	const ContentTypes& Content::type() const
	{
		return type_;
	}

	//---------------------------------------------------------------------------------------------------------
	Content::~Content()
	{

	}
}