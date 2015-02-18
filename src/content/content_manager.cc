#include "../content/content_manager.h"
#include "../content/content.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	ContentManager::ContentManager()
	{

	}

	//---------------------------------------------------------------------------------------------------------
	ContentManager* ContentManager::Instance()
	{
		static SharedPtr<ContentManager> content_manager = AllocatedMemory::Instance().Construct<ContentManager>();
		return content_manager.get();
	}

	//---------------------------------------------------------------------------------------------------------
	void ContentManager::Notify(ContentManager::Events evt, ContentTypes type, std::string path)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	ContentManager::~ContentManager()
	{

	}
}