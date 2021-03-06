#include "../application/logging.h"
#include "../application/game.h"

#ifdef SNUFF_WIN32
#include <Windows.h>
#endif


#ifdef SNUFF_BUILD_CONSOLE
#include "../console/console.h"
#endif

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	void DebugLogging::Log(const DebugLogging::LogType& type, const std::string& message, const bool& dump)
	{
    std::string msg = TypeToString(type) + " " + message + "\n";

		bool can_dump = JSStateWrapper::StackDumpAvailable();
		if (can_dump == true && (type == LogType::kError || type == LogType::kFatal) && dump == true)
		{
			msg += JSStateWrapper::Instance()->StackDump();
		}

#ifdef SNUFF_BUILD_CONSOLE
    Console* console = Console::Instance();
    console->Log(TypeToColour(type), msg);
		qApp->processEvents();
#endif

    printf(msg.c_str());

#if defined SNUFF_WIN32 && _DEBUG
		OutputDebugStringA(std::string(msg).c_str());
#endif
	}

	//---------------------------------------------------------------------------------------------------------
	void DebugLogging::Log(const std::string& message, const int& rf, const int& gf, const int& bf, const int& rb, const int& gb, const int& bb, const int& a)
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
	void DebugLogging::Assert(const std::string& msg)
	{
		Log(LogType::kFatal, msg, false);
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
	std::string DebugLogging::TypeToString(const LogType& type)
	{
		switch (type)
		{
    case LogType::kDebug: return "#";
		case LogType::kInfo: return "$";
    case LogType::kSuccess: return ">";
		case LogType::kWarning: return "?";
		case LogType::kError: return "!";
		case LogType::kFatal: return "!!!";
		default: return "unknown";
		}
	}

	//---------------------------------------------------------------------------------------------------------
	DebugLogging::LogColour DebugLogging::TypeToColour(const LogType& type)
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

  //---------------------------------------------------------------------------------------------------------
  void DebugLogging::RegisterJS(JS_SINGLETON obj)
  {
    JSFunctionRegister funcs[] = {
      { "debug", JSLogDebug },
      { "info", JSLogInfo },
      { "warning", JSLogWarning },
      { "success", JSLogSuccess },
      { "error", JSLogError },
      { "fatal", JSLogFatal },
      { "rgb", JSLogRGB }
    };

		JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
  }

  //---------------------------------------------------------------------------------------------------------
  void DebugLogging::JSLogDebug(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    Log(LogType::kDebug, wrapper.GetValue<std::string>(0, "undefined"));
  }

  //---------------------------------------------------------------------------------------------------------
  void DebugLogging::JSLogInfo(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    Log(LogType::kInfo, wrapper.GetValue<std::string>(0, "undefined"));
  }

  //---------------------------------------------------------------------------------------------------------
  void DebugLogging::JSLogWarning(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    Log(LogType::kWarning, wrapper.GetValue<std::string>(0, "undefined"));
  }

  //---------------------------------------------------------------------------------------------------------
  void DebugLogging::JSLogSuccess(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    Log(LogType::kSuccess, wrapper.GetValue<std::string>(0, "undefined"));
  }

  //---------------------------------------------------------------------------------------------------------
  void DebugLogging::JSLogError(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    Log(LogType::kError, wrapper.GetValue<std::string>(0, "undefined"));
  }

  //---------------------------------------------------------------------------------------------------------
  void DebugLogging::JSLogFatal(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    Log(LogType::kFatal, wrapper.GetValue<std::string>(0, "undefined"));
  }

  //---------------------------------------------------------------------------------------------------------
  void DebugLogging::JSLogRGB(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    Log(wrapper.GetValue<std::string>(0, "undefined"),
      wrapper.GetValue<int>(1, 255),
      wrapper.GetValue<int>(2, 255),
      wrapper.GetValue<int>(3, 255),
      wrapper.GetValue<int>(4, 128),
      wrapper.GetValue<int>(5, 128),
      wrapper.GetValue<int>(6, 128),
      wrapper.GetValue<int>(7, 255));
  }
}