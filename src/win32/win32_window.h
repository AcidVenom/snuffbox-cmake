#pragma once

#include "../platform/platform_window_base.h"

#include <string>
#include <Windows.h>

#define SNUFF_WINDOW_CENTERED -1

namespace snuffbox
{
	/**
	* @class snuffbox::Win32Window
	* @brief A Windows window for use with the engine
	* @author Daniël Konings
	*/
	class Win32Window : public IWindowBase
	{
	public:
		/**
		* @brief Constructs a Windows window
		* @param[in] x (int) The window x position
		* @param[in] y (int) The window y position
		* @param[in] w (int) The window width
		* @param[in] h (int) The window height
		* @param[in] name (std::string) The window name
		*/
		Win32Window(int x, int y, int w, int h, std::string name);

		/// Default destructor
		virtual ~Win32Window();

		/// Initialises the window
		void Initialise();

		/// @see snuffbox::IWindowBase
		void Show();

		/// @see snuffbox::IWindowBase
		void ProcessMessages();

		/// @see snuffbox::IWindowBase
		void SendQuitMessage();

		/// When the window retrieves focus execute this function
		void OnSetFocus();

		/// When the window loses focus execute this function
		void OnKillFocus();

		/**
		* @brief When a key has been pressed execute this function
		* @param[in] lParam (LPARAM) The LPARAM from the Windows message loop
		* @param[in] wParam (WPARAM) The WPARAM from the Windows message loop
		*/
		void OnKeyDown(LPARAM lParam, WPARAM wParam);

		/**
		* @brief When a key has been released execute this function
		* @param[in] lParam (LPARAM) The LPARAM from the Windows message loop
		* @param[in] wParam (WPARAM) The WPARAM from the Windows message loop
		*/
		void OnKeyUp(LPARAM lParam, WPARAM wParam);

		/**
		* @brief When a mouse button has been pressed execute this function
		* @param[in] button (int) The button that was pressed
		* @param[in] lParam (LPARAM) The LPARAM from the Windows message loop
		* @param[in] wParam (WPARAM) The WPARAM from the Windows message loop
		*/
		void OnMouseDown(int button, LPARAM lParam, WPARAM wParam);

		/**
		* @brief When a mouse button has been released execute this function
		* @param[in] button (int) The button that was pressed
		* @param[in] lParam (LPARAM) The LPARAM from the Windows message loop
		* @param[in] wParam (WPARAM) The WPARAM from the Windows message loop
		*/
		void OnMouseUp(int button, LPARAM lParam, WPARAM wParam);

		/**
		* @brief When a mouse button has been double clicked execute this function
		* @param[in] button (int) The button that was pressed
		* @param[in] lParam (LPARAM) The LPARAM from the Windows message loop
		* @param[in] wParam (WPARAM) The WPARAM from the Windows message loop
		*/
		void OnMouseDbl(int button, LPARAM lParam, WPARAM wParam);

		/** When the mouse was moved execute this function
		*	@param[in] lParam(LPARAM) The LPARAM from the Windows message loop
		* @param[in] wParam(WPARAM) The WPARAM from the Windows message loop
		*/
		void OnMouseMove(LPARAM lParam, WPARAM wParam);

		/** When the mouse wheel was used execute this function
		*	@param[in] lParam(LPARAM) The LPARAM from the Windows message loop
		* @param[in] wParam(WPARAM) The WPARAM from the Windows message loop
		*/
		void OnMouseWheel(LPARAM lParam, WPARAM wParam);

		/**
		* @brief Sets the mouse position
		* @param[in] x (int) The x position of the mouse
		* @param[in] y (int) The y position of the mouse
		*/
		void SetMousePosition(int x, int y);

		/**
		* @return const int& The x position of this window
		*/
		const int& x() const;

		/**
		* @return const int& The y position of this window
		*/
		const int& y() const;

		/**
		* @return const int& The width of this window
		*/
		const int& width() const;

		/**
		* @return const int& The height of this window
		*/
		const int& height() const;

		/**
		* @return HWND The handle to this window
		*/
		HWND handle();

		/**
		* @return const bool& Is the window currently focussed?
		*/
		const bool& focussed() const;

		/**
		* @brief Sets the width of this window
		* @param[in] w (int) The width to set
		*/
		void set_width(int w);

		/**
		* @brief Sets the height of this window
		* @param[in] h (int) The height to set
		*/
		void set_height(int h);

	private:
		int x_; //!< The x position of this window
		int y_; //!< The y position of this window
		int width_; //!< The width of this window
		int height_; //!< The height of this window
		std::string name_; //!< The name of this window
		HWND handle_; //!< The handle to this window
		HINSTANCE instance_; //!< The window instance
		bool focussed_; //!< Is the window focussed?
		POINT mouse_position_; //!< The mouse position
	};
}