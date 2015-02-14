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
#endif

    printf(msg.c_str());

#if defined SNUFF_WIN32 && _DEBUG
		OutputDebugStringA(std::string(msg).c_str());
#endif
	}

	//---------------------------------------------------------------------------------------------------------
  void DebugLogging::Log(std::string message, int rf, int gf, int bf, int rb, int gb, int bb, int a)
	{
    std::string msg = message + "\n";
		
#ifdef SNUFF_BUILD_CONSOLE
    Console* console = Console::Instance();
    console->Log(DebugLogging::LogColour{ rf, gf, bf, rb, gb, bb, a}, msg);
#endif
    
    printf(msg.c_str());

#if defined SNUFF_WIN32 && _DEBUG
    OutputDebugStringA(std::string(msg).c_str());
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
		case LogType::kDebug: return{ 128, 128, 128, 0, 0, 0, 0};
    case LogType::kInfo: return{ 255, 255, 255, 0, 0, 0, 0 };
    case LogType::kSuccess: return{ 35, 255, 0, 0, 0, 0, 0 };
    case LogType::kWarning:return{ 255, 185, 0, 0, 0, 0, 0 };
    case LogType::kError: return{ 255, 0, 0, 0, 0, 0, 0 };
    case LogType::kFatal: return{ 255, 255, 255, 255, 0, 0, 255 };
		default: return { 128, 128, 128, 0, 0, 0, 0};
		}
	}
}