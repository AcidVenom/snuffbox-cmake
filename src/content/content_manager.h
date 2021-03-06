#pragma once

#include <map>
#include "../memory/shared_ptr.h"
#include "../js/js_object.h"

#include <queue>

namespace snuffbox
{
	class Content;

	/**
	* @enum snuffbox::ContentManager::ContentTypes
	* @brief Contains different content types used by the content manager and file watch
	* @author Dani�l Konings
	*/
	enum ContentTypes
	{
		kScript,
		kTexture,
		kShader,
		kEffect,
		kMaterial,
		kModel,
		kSound,
		kCustom,
    kBox,
		kAnim,
		kParticleEffect,
    kUnknown
	};

  /**
  * @class snuffbox::ContentManager
  * @brief Used to load, unload and retrieve content within the environment
  * @author Dani�l Konings
  */
	class ContentManager : public JSObject
	{
	public:
		/**
		* @enum snuffbox::ContentManager::Events
		* @brief Contains different events for the content manager to handle
		* @author Dani�l Konings
		*/
		enum Events
		{
			kLoad,
			kReload,
			kUnload
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
		* @param[in] evt (const ContentManager::Events&) The type of event to be received
		* @param[in] type (const snuffbox::ContentTypes&) The type of the file event
		* @param[in] path (const std::string&) The path to the file that was changed
		*/
		void Notify(const Events& evt, const ContentTypes& type, const std::string& path);

		/**
		* @brief Loads a given file
		* @param[in] type (const snuffbox::ContentTypes&) The type of the content to load
		* @param[in] path (const std::string&) The path of the file to load
		*/
		void Load(const ContentTypes& type, const std::string& path);

		/**
		* @brief Reloads a given file
		* @param[in] type (const snuffbox::ContentTypes&) The type of the content to reload
		* @param[in] path (const std::string&) The path of the file to reload
		*/
		void Reload(const ContentTypes& type, const std::string& path);

		/**
		* @brief Unloads a given file
		* @param[in] type (const snuffbox::ContentTypes&) The type of the content to unload
		* @param[in] path (const std::string&) The path of the file to unload
		*/
		void Unload(const ContentTypes& type, const std::string& path);

		/**
		* @brief Watches any file for file changes
		* @param[in] path (const std::string&) The path to the file to watch
		*/
		void Watch(const std::string& path);

		/// Processes the unload queue
		void UnloadAll();

    /**
    * @brief Retrieves content by name
    * @param[in] path (const std::string&) The path to the content to be retrieved
    * @return T* A pointer to the actual content, or nullptr if not found
    */
		template<typename T>
    T* Get(const std::string& path);

    /**
    * @brief Converts a string to a content type
    * @param[in] type (const std::string&) The type to convert
    * @return snuffbox::ContentTypes The converted value
    */
    static ContentTypes StringToType(const std::string& type);

		/// Default destructor
		~ContentManager();

	private:
		std::map<std::string, SharedPtr<Content>> loaded_content_;
		std::queue<std::string> to_unload_;

	public:
		JS_NAME("ContentManager");
		static void RegisterJS(JS_SINGLETON obj);
		static void JSLoad(JS_ARGS args);
		static void JSUnload(JS_ARGS args);
		static void JSWatch(JS_ARGS args);
	};

	//---------------------------------------------------------------------------------------------------------
	template<typename T>
	inline T* ContentManager::Get(const std::string& path)
	{
		std::map<std::string, SharedPtr<Content>>::iterator it = loaded_content_.find(path);

		if (it != loaded_content_.end())
		{
			return dynamic_cast<T*>(it->second.get());
		}

		SNUFF_LOG_ERROR("Could not find content '" + path + "', are you sure it's been loaded correctly?");
		return nullptr;
	}
}