#pragma once

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
		* @return snuffbox::ContentTypes The type of this content
		*/
		ContentTypes type();

		/// Default destructor
		virtual ~Content();

	private:
		ContentTypes type_; //!< The type of this content
	};
}