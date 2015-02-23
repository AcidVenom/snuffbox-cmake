#pragma once

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcommon.h>
#include <D3DX10.h>
#include <D3DX11.h>
#include <xnamath.h>

#include <string>
#include <map>

#include "../platform/platform_render_device_base.h"
#include "../memory/shared_ptr.h"

#define SNUFF_SAFE_RELEASE(ptr, ctx) SNUFF_ASSERT_NOTNULL(ptr, ctx) ptr->Release(); ptr = nullptr;

namespace snuffbox
{
	class D3D11RenderTarget;
  class D3D11VertexBuffer;
  class D3D11InputLayout;
  class D3D11Viewport;

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

    /// Creates the backbuffer
    void CreateBackBuffer();

    /// Creates the screen quad
    void CreateScreenQuad();

    /// Creates the input layout
    void CreateInputLayout();

    /// Creates the base viewport
    void CreateBaseViewport();

		/// Finds the default adapter to use
		void FindAdapter();

		/// @see snuffbox::IRenderDeviceBase::Draw
		void Draw();

		/// @see snuffbox::IRenderDeviceBase::ResizeBuffers
		void ResizeBuffers(int w, int h);

		/// @see snuffbox::IRenderDeviceBase::Dispose
		void Dispose();

		/**
		* @brief Adds a render target to the map of render targets
		* @param[in] target (snuffbox::D3D11RenderTarget*) The render target to add
		*/
		void AddRenderTarget(D3D11RenderTarget* target);

		/**
		* @return snuffbox::D3D11RenderTarget* The swap chain of this device
		*/
		IDXGISwapChain* swap_chain();

		/**
		* @return snuffbox::D3D11RenderTarget* The device of this device
		*/
		ID3D11Device* device();

		/**
		* @return snuffbox::D3D11RenderTarget* The context of this device
		*/
		ID3D11DeviceContext* context();

		/**
		* @return snuffbox::D3D11RenderTarget* The back buffer of this device
		*/
		D3D11RenderTarget* back_buffer();

    /**
    * @return const int& The current vertex buffer type
    */
    const int& vertex_buffer_type() const;

    /**
    * @brief Sets the current vertex buffer type
    * @param[in] type (int) The type to set
    */
    void set_vertex_buffer_type(int type);

		/// Default destructor
		virtual ~D3D11RenderDevice();

	private:
		bool ready_; //!< Is the device ready?

		IDXGIAdapter* adapter_; //!< The adapter that will be used for rendering
		IDXGISwapChain* swap_chain_; //!< The swap chain this device will be created with
		ID3D11Device* device_; //!< The actual device
		ID3D11DeviceContext* context_; //!< The device context

		SharedPtr<D3D11RenderTarget> back_buffer_; //!< The backbuffer of this render device
		std::map<std::string, D3D11RenderTarget*> render_targets_; //!< The map of render targets

    SharedPtr<D3D11VertexBuffer> screen_quad_; //!< The vertex buffer of the screen quad
    int vertex_buffer_type_; //!< The current vertex buffer type

    SharedPtr<D3D11InputLayout> input_layout_; //!< The input layout

    SharedPtr<D3D11Viewport> viewport_; //!< The base viewport
	};
}