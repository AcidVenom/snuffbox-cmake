#pragma once

#include <map>
#include "../memory/shared_ptr.h"

namespace snuffbox
{
	class Content;

	/**
	* @enum snuffbox::ContentManager::ContentTypes
	* @brief Contains different content types used by the content manager and file watch
	* @author Daniël Konings
	*/
	enum ContentTypes
	{
		kScript,
		kTexture,
		kShader,
		kMaterial,
		kModel,
		kSound
	};

	class ContentManager
	{
	public:
		/**
		* @enum snuffbox::ContentManager::Events
		* @brief Contains different events for the content manager to handle
		* @author Daniël Konings
		*/
		enum Events
		{
			kLoad,
			kUnload,
			kReload
		};

		/// Default constructor
		ContentManager();

		/**
		* @brief Retrieves the singleton instance of this class
		* @return snuffbox::ContentManager* The pointer to the singleton
		*/
		static ContentManager* Instance();

		/**
		* @brief Notifies the content manager about a change in content
		* @param[in] evt (snuffbox::ContentManager::Events) The type of event to be received
		* @param[in] type (snuffbox::ContentTypes) The type of the file event
		* @param[in] path (std::string) The path to the file that was changed
		*/
		void Notify(Events evt, ContentTypes type, std::string path);

		/// Default destructor
		~ContentManager();

	private:
		std::map<std::string, SharedPtr<Content>> content;
	};
}