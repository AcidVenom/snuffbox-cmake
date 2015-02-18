#pragma once

#include <Windows.h>

#include <map>
#include <queue>

#include "../platform/platform_file_watch_base.h"

namespace snuffbox
{
	/**
	* @class snuffbox::Win32FileWatch
	* @brief Used to monitor files for changes on the Windows platform
	* @author Daniël Konings
	*/
	class Win32FileWatch : public IFileWatchBase
	{
	public:
		/**
		* @struct snuffbox::Win32FileWatch::WatchedFile
		* @brief Contains information about a watched file to monitor it
		* @author Daniël Konings
		*/ 
		struct WatchedFile
		{
			std::string path;
			ContentTypes type;
			FILETIME last_edited;
		};

		/// Default constructor
		Win32FileWatch();

		/**
		* @brief Retrieves the singleton instance of this class
		* @return snuffbox::Win32FileWatch* The pointer to the singleton
		*/
		static Win32FileWatch* Instance();

		/**
		* @see snuffbox::IFileWatchBase::Add
		*/
		bool Add(std::string path, ContentTypes type);

		/**
		* @see snuffbox::IFileWatchBase::Remove
		*/
		void Remove(std::string path);

		/**
		* @see snuffbox::IFileWatchBase::Update
		*/
		void Update();

		/**
		* @brief Retrieves the time a file was last edited
		* @param[in] path (std::string) The path to the corresponding file
		* @param[in] time (FILETIME*) The time buffer to store the resulting data in
		* @return bool Was it a success or not?
		*/
		bool GetLastEditedTime(std::string path, FILETIME* time);

		/// Processes the queue and adds new files into the map
		void Process();

		/**
		* @see snuffbox::IFileWatchBase::last_reloaded
		*/
		const std::string& last_reloaded() const;

		/// Default destructor
		virtual ~Win32FileWatch();

	private:
		typedef std::map<std::string, WatchedFile> FileMap;
		FileMap watched_files_; //!< A map of watched files by path
		std::queue<WatchedFile> queue_; //!< A queue to add files to without interfering with the content manager
		std::queue<std::string> to_remove_; //!< A queue to remove files without interfering with the content manager
		std::string last_reloaded_; //!< The last reloaded file
	};
}