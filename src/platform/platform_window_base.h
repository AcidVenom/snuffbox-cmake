#pragma once

#include <string>

namespace snuffbox
{
	/**
	* @class snuffbox::IWindowBase
	* @brief The window base every window should derive from
	* @author Daniël Konings
	*/
	class IWindowBase
	{
	public:
		/// Initialises a window
		virtual void Initialise() = 0;
		
		/// Shows a window
		virtual void Show() = 0;

    /**
    * @brief Sets the size of the window
    * @param[in] w (const int&) The window width
    * @param[in] h (const int&) The window height
    */
    virtual void SetSize(const int& w, const int& h) = 0;

    /**
    * @brief Sets the name of the window
    * @param[in] name (const std::string&) The name to set
    */
    virtual void SetName(const std::string& name) = 0;

		/// Process the message queue, send to the input handlers
		virtual void ProcessMessages() = 0;

		/// Post a quit message to the game instance
		virtual void SendQuitMessage() = 0;

	};
}