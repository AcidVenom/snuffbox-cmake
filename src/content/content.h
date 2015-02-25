#pragma once

#include <string>

namespace snuffbox
{
	enum ContentTypes;

	/**
	* @class snuffbox::Content
	* @brief The base class of every content type
	* @author Daniël Konings
	*/
	class Content
	{
	public:
		/// Default constructor
		Content(ContentTypes type);

		/**
		* @brief Loads this piece of content from a given path
		* @param[in] path (std::string) The path to the content to load
		*/
		virtual void Load(std::string path);

		/**
		* @return snuffbox::ContentTypes The type of this content
		*/
		ContentTypes type();

		/// Default destructor
		virtual ~Content();

	private:
		ContentTypes type_; //!< The type of this content
	};
}