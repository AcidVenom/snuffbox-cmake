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

		/// Disposes the render device, releasing all resources
		virtual void Dispose() = 0;
	};
}