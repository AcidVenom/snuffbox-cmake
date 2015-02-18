#pragma once

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcommon.h>
#include <D3DX10.h>
#include <D3DX11.h>
#include <xnamath.h>

#include "../platform/platform_render_device_base.h"

namespace snuffbox
{
	/**
	* @class snuffbox::D3D11RenderDevice
	* @brief A Direct3D 11 render device to display graphics with
	* @author Daniël Konings
	*/
	class D3D11RenderDevice : public IRenderDeviceBase
	{
	public:
		/// Default constructor
		D3D11RenderDevice();

		/**
		* @brief Retrieves the singleton instance of this class
		* @return D3D11RenderDevice* The pointer to the singleton
		*/
		static D3D11RenderDevice* Instance();

		/// @see snuffbox::IRenderDeviceBase::Initialise
		bool Initialise();

		/// @see snuffbox::IRenderDeviceBase::Draw
		void Draw();

		/// @see snuffbox::IRenderDeviceBase::Dispose
		void Dispose();

		/// Default destructor
		virtual ~D3D11RenderDevice();
	};
}