#pragma once

#include <string>
#include "../content/content_manager.h"

namespace snuffbox
{
	/**
	* @class snuffbox::IFileWatchBase
	* @brief The file watch base every file watch should derive from
	* @author Daniël Konings
	*/
	class IFileWatchBase
	{
	public:

		/**
		* @brief Adds a file to the watch
		* @param[in] path (const std::string&) The path to the file
		* @param[in] type (const snuffbox::ContentTypes&) The file type
		* @return bool Was it a success or not?
		*/
		virtual bool Add(const std::string& path, const ContentTypes& type) = 0;

		/**
		* @brief Removes a file from the watch
		* @param[in] path (const std::string&) The file to remove from the file watch
		*/
		virtual void Remove(const std::string& path) = 0;

		/// Updates the file watch, checks for file changes
		virtual void Update() = 0;

		/// Processes the file watch queues, to prevent interfering with the content manager
		virtual void Process() = 0;

		/**
		* @return std::string The last reloaded file
		*/
		virtual const std::string& last_reloaded() const = 0;
	};
}