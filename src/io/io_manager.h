#pragma once

#include "../js/js_object.h"

#include <vector>

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
		* @param[in] path (const std::string& path) The path to the file
		* @param[out] buffer (std::string*) The buffer to write to
		* @return bool Was it a success or not?
		*/
		bool Read(const std::string& path, std::string* buffer);

		/**
		* @brief Checks if a file exists
		* @param[in] path (const std::string&) The path to the file
		* @return bool Was the file found?
		*/
		bool Exists(const std::string& path);

		/**
		* @brief Writes to a given location
		* @param[in] path (const std::string&) The path to write to
		* @param[in] src (const std::string&) The data to write
		* @return bool Was it a success?
		*/
		bool Write(const std::string& path, const std::string& src);

		/**
		* @brief Does a given directory exist?
		* @param[in] path (const std::string&) The path to the directory
		* @return bool The boolean value
		*/
		bool DirectoryExists(const std::string& path);

		/**
		* @brief Creates a directory
		* @param[in] path (const std::string&) The path to the directory to create
		*/
		void CreateDir(const std::string& path);

		/**
		* @brief Retrieves the files in a directory and returns the names
		* @param[in] path (const std::string&) The path of the directory to look for files
		* @return std::vector<std::string> The list of file names
		*/
		std::vector<std::string> FilesInDirectory(const std::string& path);

	public:
		JS_NAME("IO");
		static void RegisterJS(JS_SINGLETON obj);
		static void JSRead(JS_ARGS args);
		static void JSExists(JS_ARGS args);
		static void JSWrite(JS_ARGS args);
		static void JSDirectoryExists(JS_ARGS args);
		static void JSCreateDir(JS_ARGS args);
		static void JSFilesInDirectory(JS_ARGS args);
	};
}