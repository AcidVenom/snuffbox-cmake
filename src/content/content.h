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
		/**
		* @brief Construct by type
		* @param[in] type (const snuffbox::ContentTypes&) The type to construct with
		*/
		Content(const ContentTypes& type);

		/**
		* @brief Loads this piece of content from a given path
		* @param[in] path (const std::string&) The path to the content to load
		*/
		virtual void Load(const std::string& path);

		/// Validates this piece of content
		void Validate();

		/// Invalidates this piece of content
		void Invalidate();

		/**
		* @return const snuffbox::ContentTypes& The type of this content
		*/
		const ContentTypes& type() const;

		/**
		* @return const bool& Is this content still valid?
		*/
		const bool& is_valid() const;

		/// Default destructor
		virtual ~Content();

	private:
		ContentTypes type_; //!< The type of this content
		bool is_valid_; //!< Is this content still valid?
	};
}