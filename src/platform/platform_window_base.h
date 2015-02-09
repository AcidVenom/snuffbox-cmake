#pragma once

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

		/// Process the message queue, send to the input handlers
		virtual void ProcessMessages() = 0;

		/// Post a quit message to the game instance
		virtual void SendQuitMessage() = 0;

	};
}