#pragma once

#include "../js/js_object.h"

namespace snuffbox
{
	class IOManager : public JSObject
	{ 
	public:
		/// Default constructor
		IOManager();

		/**
		* @brief Retrieves the singleton instance of this class
		* @return snuffbox::IOManager* The pointer to the singleton
		*/
		static IOManager* Instance();

		/// Default destructor
		virtual ~IOManager();

		/**
		* @brief Reads the contents of a file
		* @param[in] path (std::string) The path to the file
		* @param[in] buffer (std::string*) The buffer to write to
		* @return bool Was it a success or not?
		*/
		bool Read(std::string path, std::string* buffer);

		/**
		* @brief Checks if a file exists
		* @param[in] path (std::string) The path to the file
		* @return bool Was the file found?
		*/
		bool Exists(std::string path);

	public:
		JS_NAME("IO");
		static void RegisterJS(JS_SINGLETON obj);
		static void JSRead(JS_ARGS args);
		static void JSExists(JS_ARGS args);
	};
}