#pragma once

#ifdef SNUFF_OSX
#include "../osx/osx_text_file.h"
namespace snuffbox { typedef OSXTextFile PlatformTextFile; }
#elif defined SNUFF_WIN32
#include "../win32/win32_text_file.h"
namespace snuffbox { typedef Win32TextFile PlatformTextFile; }
#elif defined SNUFF_LINUX
#include "../linux/linux_text_file.h"
namespace snuffbox { typedef LinuxTextFile PlatformTextFile; }
#endif

namespace snuffbox
{
	class TextFile : public PlatformTextFile
	{
	public:
		/// Default constructor
		TextFile(){}

		/// Default destructor
		virtual ~TextFile(){}
	};
}