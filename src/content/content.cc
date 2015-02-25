#include "../content/content.h"
#include "../content/content_manager.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	Content::Content(ContentTypes type)
		: type_(type)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	void Content::Load(std::string path)
	{
		SNUFF_LOG_WARNING("Attempted to load '" + path + "', but no load functionality was provided");
	}

	//---------------------------------------------------------------------------------------------------------
	ContentTypes Content::type()
	{
		return type_;
	}

	//---------------------------------------------------------------------------------------------------------
	Content::~Content()
	{

	}
}