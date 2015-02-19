#pragma once

#include "../d3d11/d3d11_render_device.h"
#include "../js/js_object.h"

namespace snuffbox
{
	/**
	* @class snuffbox::D3D11RenderTarget
	* @brief Contains a Direct3D 11 back buffer object for use with the render device
	* @author Daniël Konings
	*/
	class D3D11RenderTarget : public JSObject
	{
	public:
		/**
		* @enum snuffbox::D3D11RenderTarget::RenderTargets
		* @brief Contains different types of render targets
		* @author Daniël Konings
		*/
		enum RenderTargets
		{
			kBackBuffer,
			kRenderTarget
		};
	public:
		/**
		* @brief Construct with a name
		* @param[in] name (std::string) The name of this target
		*/
		D3D11RenderTarget(std::string name);

		/// JavaScript constructor
		D3D11RenderTarget(JS_ARGS args);

		/**
		* @brief Creates the render target by type
		* @param[in] type (snuffbox::D3D11RenderTarget::RenderTargets) The type of the render target
		* @param[in] swap_chain (IDXGISwapChain*) The swap chain to create the target with
		* @param[in] device (ID3D11Device*) The device to create the target with
		*/
		void Create(RenderTargets type, IDXGISwapChain* swap_chain, ID3D11Device* device);

		/**
		* @brief Retrieves the description of the view of this render target
		* @param[in] desc (D3D11_RENDER_TARGET_VIEW_DESC*) The buffer to write to
		* @return bool Was it a success or not?
		*/
		bool GetDescription(D3D11_RENDER_TARGET_VIEW_DESC* desc);

		/**
		* @brief Retrieves the description of the buffer of this render target
		* @param[in] desc (D3D11_TEXTURE2D_DESC*) The buffer to write to
		* @return bool Was it a success or not?
		*/
		bool GetDescription(D3D11_TEXTURE2D_DESC* desc);

		/**
		* @brief Clears the render target
		* @param[in] context ID3D11DeviceContext* The context to clear the target with
		*/
		void Clear(ID3D11DeviceContext* context);

		/**
		* @brief Sets the render target
		* @param[in] context ID3D11DeviceContext* The context to set the target with
		*/
		void Set(ID3D11DeviceContext* context);

		/**
		* @return const std::string& The name of this render target
		*/
		const std::string& name() const;

		/// Default destructor
		virtual ~D3D11RenderTarget();

	private:
		bool valid_; //!< Is this render target valid?
		std::string name_; //!< The name of this render target
		RenderTargets type_; //!< The type of this render target
		ID3D11Texture2D* buffer_; //!< The buffer of this render target
		ID3D11RenderTargetView* view_; //!< The actual view of this render target
		ID3D11ShaderResourceView* resource_; //!< When used as a normal render target, use this to store the texture

	public:
		JS_NAME("RenderTarget");
		static void RegisterJS(JS_CONSTRUCTABLE obj);
	};
}