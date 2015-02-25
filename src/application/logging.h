#pragma once

#include <assert.h>
#include <string>

#include "../js/js_object.h"

#define SNUFF_LOG_INFO(msg) snuffbox::DebugLogging::Log(snuffbox::DebugLogging::LogType::kInfo,##msg)
#define SNUFF_LOG_DEBUG(msg) snuffbox::DebugLogging::Log(snuffbox::DebugLogging::LogType::kDebug,##msg)
#define SNUFF_LOG_WARNING(msg) snuffbox::DebugLogging::Log(snuffbox::DebugLogging::LogType::kWarning,##msg)
#define SNUFF_LOG_SUCCESS(msg) snuffbox::DebugLogging::Log(snuffbox::DebugLogging::LogType::kSuccess,##msg)
#define SNUFF_LOG_ERROR(msg) snuffbox::DebugLogging::Log(snuffbox::DebugLogging::LogType::kError,##msg)
#define SNUFF_LOG_FATAL(msg) snuffbox::DebugLogging::Log(snuffbox::DebugLogging::LogType::kFatal,##msg)
#define SNUFF_LOG_RGB(msg, r1, g1, b1, r2, g2, b2, a) snuffbox::DebugLogging::Log(##msg,##r1,##g1,##b1,##r2,##g2,##b2,##a)

#ifdef SNUFF_OSX
#define SNUFF_BREAK
#elif defined SNUFF_WIN32
#include <intrin.h>
#define SNUFF_BREAK	while(true){ __debugbreak(); }
#elif defined SNUFF_LINUX
#define SNUFF_BREAK
#endif

#define SNUFF_ASSERT(msg, ctx) {std::string message = ##msg; std::string result = "\n\nSnuffbox assertion!\n----------------------\n" + message + "\n\n" + ##ctx + ":" + std::to_string(__LINE__); SNUFF_LOG_FATAL(result); DebugLogging::Break();}
#define SNUFF_XASSERT(expr, msg, ctx) if (!(expr)){ SNUFF_ASSERT(msg, ctx) }
#define SNUFF_ASSERT_NOTNULL(ptr, ctx) SNUFF_XASSERT(ptr != nullptr, "Attempt to get a null pointer!", ctx)

namespace snuffbox
{
	/**
	* @class snuffbox::DebugLogging
	* @brief A handful of functions for debug logging with an associated severity, this logs to the console, VS output window and the snuffbox console
	* @author Daniël Konings
	*/
	class DebugLogging : public JSObject
	{
	public:
		/**
		* @struct snuffbox::DebugLogging::LogColour
		* @brief A structure to hold colour data for logging
		* @author Daniël Konings
		*/
		struct LogColour
		{
			int rf, gf, bf;
      int rb, gb, bb;
      int a;
		};

		/**
		* @enum snuffbox::DebugLogging::LogType
		* @brief Log types for different colour coding
		* @author Daniël Konings
		*/
		enum LogType
		{
			kInfo,
			kDebug,
			kWarning,
			kSuccess,
			kError,
			kFatal,
			kRGB
		};
	public:
		/**
		* @brief Logs with a given logging type and a message
		* @param[in] type (const snuffbox::DebugLogging::LogType&) The type of the log message
		* @param[in] msg (const std::string&) The message to log
		*/
		static void Log(const LogType& type, const std::string& msg);

		/**
		* @brief Logs with a given RGB value and a message
		* @param[in] msg (const std::string&) The message to log
		* @param[in] rf (const int&) The R value of the foreground colour
		* @param[in] gf (const int&) The G value of the foreground colour
		* @param[in] bf (const int&) The B value of the foreground colour
    * @param[in] rb (const int&) The R value of the background colour
    * @param[in] gb (const int&) The G value of the background colour
    * @param[in] bb (const int&) The B value of the background colour
    * @param[in] a  (const int&) The alpha value of the background colour
		*/
		static void Log(const std::string& msg, const int& rf, const int& gf, const int& bf, const int& rb, const int& gb, const int& bb, const int& a);

    /// Halts the runtime
    static void Break();

		/**
		* @brief Converts a logging type to a string
		* @param[in] type (const snuffbox::DebugLogging::LogType&) The type to convert
		* @return std::string The converted string
		*/
		static std::string TypeToString(const LogType& type);

		/**
		* @brief Converts a logging type to a colour
		* @param[in] type (const snuffbox::DebugLogging::LogType&) The type to convert
		* @return snuffbox::DebugLogging::LogColour The converted colour
		*/
		static LogColour TypeToColour(const LogType& type);

  public:
    JS_NAME("Log");
    static void RegisterJS(JS_SINGLETON obj);

    static void JSLogDebug(JS_ARGS args);
    static void JSLogInfo(JS_ARGS args);
    static void JSLogWarning(JS_ARGS args);
    static void JSLogSuccess(JS_ARGS args);
    static void JSLogError(JS_ARGS args);
    static void JSLogFatal(JS_ARGS args);
    static void JSLogRGB(JS_ARGS args);
	};
}