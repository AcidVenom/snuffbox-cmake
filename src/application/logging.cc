#include "../application/logging.h"

#ifdef SNUFF_WIN32
#include <Windows.h>
#endif


#ifdef SNUFF_BUILD_CONSOLE
#include "../console/console.h"
#endif

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	void DebugLogging::Log(DebugLogging::LogType type, std::string message)
	{
		std::string msg = "[" + TypeToString(type) + "] " + message + "\n";

#ifdef SNUFF_BUILD_CONSOLE
    Console* console = Console::Instance();
    console->Log(TypeToColour(type), msg);
#else
		printf(msg.c_str());
#endif

#ifdef SNUFF_WIN32
		OutputDebugStringA(std::string(msg).c_str());
#endif
	}

	//---------------------------------------------------------------------------------------------------------
	void DebugLogging::Log(std::string message, float rb, float gb, float bb, float rf, float gf, float bf)
	{
    std::string msg = message + "\n";
		
#ifdef SNUFF_BUILD_CONSOLE
    Console* console = Console::Instance();
    console->Log(DebugLogging::LogColour{ rb, gb, bb, rf, gf, bf }, msg);
#else
    printf(msg.c_str());
#endif
	}

  //---------------------------------------------------------------------------------------------------------
  void DebugLogging::Break()
  {
#ifdef SNUFF_BUILD_CONSOLE
    Console* console = Console::Instance(); 
    while (console->IsVisible() == true && console->enabled() == true){ 
      qApp->processEvents(); 
    }
#endif

    while (true)
    {
      SNUFF_BREAK;
    }
  }

	//---------------------------------------------------------------------------------------------------------
	std::string DebugLogging::TypeToString(LogType type)
	{
		switch (type)
		{
		case LogType::kDebug: return "Debug";
		case LogType::kInfo: return "Info";
		case LogType::kSuccess: return "Success";
		case LogType::kWarning: return "Warning";
		case LogType::kError: return "Error";
		case LogType::kFatal: return "FATAL";
		case LogType::kRGB: return "RGB";
		default: return "unknown";
		}
	}

	//---------------------------------------------------------------------------------------------------------
	DebugLogging::LogColour DebugLogging::TypeToColour(LogType type)
	{
		switch (type)
		{
		case LogType::kDebug: return{ 128.0f, 128.0f, 128.0f, 0.0f, 0.0f, 0.0f};
		case LogType::kInfo: return { 255.0f, 255.0f, 255.0f, 30.0f, 30.0f, 30.0f};
		case LogType::kSuccess: return{ 35.0f, 255.0f, 0.0f, 70.0f, 120.0f, 55.0f};
		case LogType::kWarning:return{ 255.0f, 185.0f, 0.0f, 70.0f, 65.0f, 15.0f};
		case LogType::kError: return{ 255.0f, 0.0f, 0.0f, 60.0f, 0.0f, 0.0f};
		case LogType::kFatal: return{ 255.0f, 255.0f, 255.0f, 255.0f, 0.0f, 0.0f};
		default: return { 128.0f, 128.0f, 128.0f, 0.0f, 0.0f, 0.0f};
		}
	}
}