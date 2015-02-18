#include "../win32/win32_file_watch.h"

#include "../memory/shared_ptr.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	Win32FileWatch::Win32FileWatch()
	{

	}

	//---------------------------------------------------------------------------------------------------------
	Win32FileWatch* Win32FileWatch::Instance()
	{
		static SharedPtr<Win32FileWatch> file_watch = AllocatedMemory::Instance().Construct<Win32FileWatch>();
		return file_watch.get();
	}

	//---------------------------------------------------------------------------------------------------------
	bool Win32FileWatch::Add(std::string path, ContentTypes type)
	{
		WatchedFile file;

		bool success = GetLastEditedTime(path, &file.last_edited);
		file.type = type;
		file.path = path;

		if (success == false)
		{
			SNUFF_LOG_ERROR("Could not add '" + path + "' to the file watch, file will not be hot reloaded");
			return false;
		}
		
		FileMap::iterator it = watched_files_.find(path);

		if (it == watched_files_.end())
		{
			queue_.push(file);
			return true;
		}

		SNUFF_LOG_WARNING("File '" + path + "' was already added to the file watch earlier, file will still be hot reloaded");
		return true;
	}

	//---------------------------------------------------------------------------------------------------------
	void Win32FileWatch::Remove(std::string path)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	void Win32FileWatch::Update()
	{
		bool success = false;
		FILETIME time;
		ContentManager* content_manager = ContentManager::Instance();

		for (FileMap::iterator& it = watched_files_.begin(); it != watched_files_.end(); ++it)
		{
			WatchedFile& file = it->second;
			success = GetLastEditedTime(file.path, &time);

			if (success == false)
			{
				SNUFF_LOG_ERROR("Could not retrieve the last edited time for file '" + file.path + "'");
				continue;
			}

			if (file.last_edited.dwLowDateTime != time.dwLowDateTime || file.last_edited.dwHighDateTime != time.dwHighDateTime)
			{
				file.last_edited = time;
				content_manager->Notify(ContentManager::Events::kReload, file.type, file.path);
			}
		}
	}

	//---------------------------------------------------------------------------------------------------------
	bool Win32FileWatch::GetLastEditedTime(std::string path, FILETIME* time)
	{
		return true;
	}

	//---------------------------------------------------------------------------------------------------------
	void Win32FileWatch::Process()
	{
		while (queue_.empty() == false)
		{
			const WatchedFile& top = queue_.front();

			watched_files_.emplace(top.path, top);

			queue_.pop();
		}
	}

	//---------------------------------------------------------------------------------------------------------
	Win32FileWatch::~Win32FileWatch()
	{

	}
}