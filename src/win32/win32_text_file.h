#pragma once

#include "../platform/platform_text_file_base.h"

namespace snuffbox
{
	/**
	* @class snuffbox::Win32TextFile
	* @brief A Windows specific text file class
	* @author Daniël Konings
	*/
	class Win32TextFile : public ITextFileBase
	{
	public:
		/// Default constructor
		Win32TextFile();

		/// Default destructor
		virtual ~Win32TextFile();

		/// @see snuffbox::ITextFileBase
		bool Open(const std::string& path);

		/// @see snuffbox::ITextFileBase
		std::string Read();

		/// @see snuffbox::ITextFileBase
		bool Write(const std::string& path, const std::string& src);

	private:
		std::string path_; //!< The current path in use
		bool valid_; //!< Is this text file valid?
	};
}