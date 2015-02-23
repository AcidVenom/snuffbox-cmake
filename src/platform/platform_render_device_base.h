#pragma once

namespace snuffbox
{
	/**
	* @class snuffbox::IRenderDeviceBase
	* @brief The base class that every render device is supposed to derive from
	* @author Daniël Konings
	*/
	class IRenderDeviceBase
	{
		/**
		* @brief Initialises the render device
		* @return bool Was it a success or not?
		*/
		virtual bool Initialise() = 0;

		/// Render to the window
		virtual void Draw() = 0;

		/**
		* @brief Resize buffers when window is resized
		* @param[in] w (int) The new width
		* @param[in] h (int) The new height
		*/
		virtual void ResizeBuffers(int w, int h) = 0;

		/// Disposes the render device, releasing all resources
		virtual void Dispose() = 0;
	};
}