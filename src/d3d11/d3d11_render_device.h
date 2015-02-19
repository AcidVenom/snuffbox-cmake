#pragma once

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcommon.h>
#include <D3DX10.h>
#include <D3DX11.h>
#include <xnamath.h>

#include <string>

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

		/**
		* @brief Converts a HRESULT to a C string
		* @param[in] hr (HRESULT) The HRESULT to convert
		* @param[in] context (std::string) The context this error occured in
		* @return (std::string) The converted string
		*/
		std::string HRToString(HRESULT hr, std::string context);

		/// @see snuffbox::IRenderDeviceBase::Initialise
		bool Initialise();

		/// Creates the device
		void CreateDevice();

		/// Finds the default adapter to use
		void FindAdapter();

		/// @see snuffbox::IRenderDeviceBase::Draw
		void Draw();

		/// @see snuffbox::IRenderDeviceBase::Dispose
		void Dispose();

		/// Default destructor
		virtual ~D3D11RenderDevice();

	private:
		IDXGIAdapter* adapter_; //!< The adapter that will be used for rendering
		IDXGISwapChain* swap_chain_; //!< The swap chain this device will be created with
		ID3D11Device* device_; //!< The actual device
		ID3D11DeviceContext* context_; //!< The device context
	};
}