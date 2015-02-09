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
		bool Open(std::string path);

		/// @see snuffbox::ITextFileBase
		std::string Read();

	private:
		std::string path_;
		bool valid_;
	};
}