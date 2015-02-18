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
		/**
		* @brief Construct with given parameters
		* @param[in] x (int) The x position of the window
		* @param[in] y (int) The y position of the window
		* @param[in] w (int) The width of the window
		* @param[in] h (int) The height of the window
		* @param[in] name (std::string) The name of the window
		*/
		Window(int x, int y, int w, int h, std::string name) :
			PlatformWindow(x, y, w, h, name)
		{

		};

		/// Default destructor
		virtual ~Window()
		{

		};
	};
}