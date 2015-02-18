#pragma once

#ifdef SNUFF_OSX
#include "../osx/osx_file_watch.h"
namespace snuffbox { typedef OSXFileWatch PlatformFileWatch; }
#elif defined SNUFF_WIN32
#include "../win32/win32_file_watch.h"
namespace snuffbox { typedef Win32FileWatch PlatformFileWatch; }
#elif defined SNUFF_LINUX
#include "../linux/linux_file_watch.h"
namespace snuffbox { typedef LinuxFileWatch PlatformFileWatch; }
#endif

namespace snuffbox
{
	class FileWatch : public PlatformFileWatch
	{
	public:
		/// Default constructor
		FileWatch(){};

		/// Default destructor
		virtual ~FileWatch(){};
	};
}