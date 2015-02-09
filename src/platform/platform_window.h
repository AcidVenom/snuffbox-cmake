#pragma once

#ifdef SNUFF_OSX
#include "../osx/osx_window.h"
namespace snuffbox { typedef OSXWindow PlatformWindow; }
#elif defined SNUFF_WIN32
#include "../win32/win32_window.h"
namespace snuffbox { typedef Win32Window PlatformWindow; }
#elif defined SNUFF_LINUX
#include "../linux/linux_window.h"
namespace snuffbox { typedef LinuxWindow PlatformWindow; }
#endif

namespace snuffbox
{
	class Window : public PlatformWindow
	{
	public:
		Window(int x, int y, int w, int h, std::string name) :
			PlatformWindow(x, y, w, h, name)
		{

		};
		virtual ~Window()
		{

		};
	};
}