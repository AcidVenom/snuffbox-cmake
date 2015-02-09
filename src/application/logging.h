#pragma once

#include <assert.h>
#include <string>

#define SNUFF_LOG_INFO(msg) snuffbox::DebugLogging::Log(snuffbox::DebugLogging::LogType::kInfo,##msg)
#define SNUFF_LOG_DEBUG(msg) snuffbox::DebugLogging::Log(snuffbox::DebugLogging::LogType::kDebug,##msg)
#define SNUFF_LOG_WARNING(msg) snuffbox::DebugLogging::Log(snuffbox::DebugLogging::LogType::kWarning,##msg)
#define SNUFF_LOG_SUCCESS(msg) snuffbox::DebugLogging::Log(snuffbox::DebugLogging::LogType::kSuccess,##msg)
#define SNUFF_LOG_ERROR(msg) snuffbox::DebugLogging::Log(snuffbox::DebugLogging::LogType::kError,##msg)
#define SNUFF_LOG_FATAL(msg) snuffbox::DebugLogging::Log(snuffbox::DebugLogging::LogType::kFatal,##msg)
#define SNUFF_LOG_RGB(msg, r1, g1, b1, r2, g2, b2) snuffbox::DebugLogging::Log(##msg,##r1,##g1,##b1,##r2,##g2,##b2)

#ifdef SNUFF_OSX
#define SNUFF_BREAK
#elif defined SNUFF_WIN32
#include <intrin.h>
#define SNUFF_BREAK	__debugbreak();
#elif defined SNUFF_LINUX
#define SNUFF_BREAK
#endif

#define SNUFF_ASSERT(msg, ctx) std::string message = ##msg; std::string result = "\n\nSnuffbox assertion!\n----------------------\n" + message + "\n\n" + ##ctx + ":" + std::to_string(__LINE__); SNUFF_LOG_FATAL(result); while(true){ SNUFF_BREAK; }
#define SNUFF_XASSERT(expr, msg, ctx) if (!(expr)){ SNUFF_ASSERT(msg, ctx) }
#define SNUFF_ASSERT_NOTNULL(ptr, ctx) SNUFF_XASSERT(ptr != nullptr, "Attempt to get a null pointer!", ctx)

namespace snuffbox
{
	/**
	* @class snuffbox::DebugLogging
	* @brief A handful of functions for debug logging with an associated severity, this logs to the console, VS output window and the snuffbox console
	* @author Daniël Konings
	*/
	class DebugLogging
	{
	public:
		/**
		* @struct snuffbox::DebugLogging::LogColour
		* @brief A structure to hold colour data for logging
		* @author Daniël Konings
		*/
		struct LogColour
		{
			float rb, gb, bb;
			float rf, gf, bf;
			float a;
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
		* @param[in] type (snuffbox::DebugLogging::LogType) The type of the log message
		* @param[in] msg (std::string) The message to log
		*/
		static void Log(LogType type, std::string msg);

		/**
		* @brief Logs with a given RGB value and a message
		* @param[in] msg (std::string) The message to log
		* @param[in] float (rb) The R value of the background colour
		* @param[in] float (gb) The G value of the background colour
		* @param[in] float (bb) The B value of the background colour
		* @param[in] float (rf) The R value of the foreground colour
		* @param[in] float (gf) The G value of the foreground colour
		* @param[in] float (bf) The B value of the foreground colour
		*/
		static void Log(std::string msg, float rb, float gb, float bb, float rf, float gf, float bf);

		/**
		* @brief Converts a logging type to a string
		* @param[in] type (snuffbox::DebugLogging::LogType) The type to convert
		* @return std::string The converted string
		*/
		static std::string TypeToString(LogType type);

		/**
		* @brief Converts a logging type to a colour
		* @param[in] type (snuffbox::DebugLogging::LogType) The type to convert
		* @return snuffbox::DebugLogging::LogColour The converted colour
		*/
		static LogColour TypeToColour(LogType type);
	};
}