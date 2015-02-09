#pragma once

#include <string>

namespace snuffbox
{
	/**
	* @class snuffbox::ITextFileBase
	* @brief The text file base every text file should derive from
	* @author Daniël Konings
	*/
	class ITextFileBase
	{
	public:

		/**
		* @brief Opens a text file given a specific path
		* @param[in] path (std::string) The path to the text file
		* @return bool Was it a success or not?
		*/
		virtual bool Open(std::string path) = 0;

		/*
		* @return std::string All data contained in the text file as a string
		*/
		virtual std::string Read() = 0;
	};
}