#include <fstream>

#include "../win32/win32_text_file.h"

#include "../application/logging.h"

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	Win32TextFile::Win32TextFile() :
		valid_(false),
		path_("undefined")
	{
		
	}

	//-------------------------------------------------------------------------------------------
	bool Win32TextFile::Open(const std::string& path)
	{
		path_ = path;
		std::fstream fin(path);

		if (!fin)
		{
			return false;
		}
		
		valid_ = true;
		return true;
	}

	//-------------------------------------------------------------------------------------------
	std::string Win32TextFile::Read()
	{
		SNUFF_XASSERT(valid_ == true, "File was not succesfully opened initially, aborting! (" + path_ + ")", "Win32TextFile::Read");
		std::string contents = "";

		char ch;
		std::fstream fin(path_);

		while (fin >> std::noskipws >> ch)
		{
			contents += ch;
		}
		
		return contents;
	}

	//-------------------------------------------------------------------------------------------
	bool Win32TextFile::Write(const std::string& path, const std::string& src)
	{
		std::ofstream out(path);

		if (!out)
		{
			SNUFF_LOG_ERROR("Could not save to location '" + path + "'");
			return false;
		}

		out << std::noskipws << src;

		out.close();
		return true;
	}

	//-------------------------------------------------------------------------------------------
	Win32TextFile::~Win32TextFile()
	{

	}
}