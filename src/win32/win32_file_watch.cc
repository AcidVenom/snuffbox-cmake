#include "../win32/win32_file_watch.h"

#include "../memory/shared_ptr.h"

#include "../cvar/cvar.h"
#include "../application/game.h"

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
	bool Win32FileWatch::Add(const std::string& path, const ContentTypes& type)
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
	void Win32FileWatch::Remove(const std::string& path)
	{
		FileMap::iterator it = watched_files_.find(path);

		if (it == watched_files_.end())
		{
			SNUFF_LOG_ERROR("Attempted to remove file '" + path + "' from the file watch, but it was never added");
			return;
		}

		to_remove_.push(path);
	}

	//---------------------------------------------------------------------------------------------------------
	void Win32FileWatch::Update()
	{
		bool success = false;
		FILETIME time;
		FILETIME last_edited;
		ContentManager* content_manager = ContentManager::Instance();

		for (FileMap::iterator& it = watched_files_.begin(); it != watched_files_.end(); ++it)
		{
			WatchedFile& file = it->second;
			success = GetLastEditedTime(file.path, &time);

			if (success == false)
			{
				continue;
			}

			last_edited = file.last_edited;
			if (last_edited.dwLowDateTime != time.dwLowDateTime || last_edited.dwHighDateTime != time.dwHighDateTime)
			{
				file.last_edited = time;
				last_reloaded_ = file.path;
				content_manager->Notify(ContentManager::Events::kReload, file.type, file.path);
				Game::Instance()->Notify(Game::GameNotifications::kReload);
			}
		}
	}

	//---------------------------------------------------------------------------------------------------------
	bool Win32FileWatch::GetLastEditedTime(const std::string& path, FILETIME* time)
	{
		FILETIME creation_time;
		FILETIME last_access_time;

		std::string full_path = Game::Instance()->path() + "/" + path;

		HANDLE file = CreateFileA(full_path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

		BOOL result = GetFileTime(file, &creation_time, &last_access_time, time);

		CloseHandle(file);
		return result == TRUE;
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

		while (to_remove_.empty() == false)
		{
			const std::string& top = to_remove_.front();

			watched_files_.erase(watched_files_.find(top));

			to_remove_.pop();
		}
	}

	//---------------------------------------------------------------------------------------------------------
	const std::string& Win32FileWatch::last_reloaded() const
	{
		return last_reloaded_;
	}

	//---------------------------------------------------------------------------------------------------------
	Win32FileWatch::~Win32FileWatch()
	{

	}
}