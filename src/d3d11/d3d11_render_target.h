#pragma once

#include "../d3d11/d3d11_render_device.h"
#include "../js/js_object.h"

namespace snuffbox
{
  class D3D11RenderQueue;
	class D3D11Uniforms;
  class D3D11RenderElement;

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
    * @param[in] w (const int&) The width of the new render target, if none is provided, it defaults to the back buffer width
    * @param[in] h (const int&) The width of the new render target, if none is provided, it defaults to the back buffer height
		*/
		void Create(RenderTargets type, IDXGISwapChain* swap_chain, ID3D11Device* device, const int& w = -1, const int& h = -1);

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
    * @param[in] depth_stencil ID3D11DepthStencilView* The depth stencil view to do depth tests with, defaults to NULL
		*/
    void Set(ID3D11DeviceContext* context, ID3D11DepthStencilView* depth_stencil = NULL);

    /** 
    * @brief Draws to this render target
    * @param[in] context ID3D11DeviceContext* The context to use for rendering
    */
    void Draw(ID3D11DeviceContext* context);

		/// Clears the render target of render elements
		void Clear();

    /**
    * @brief Remaps the default viewport of this render target
    * @param[in] x (const float&) The x position of the viewport
    * @param[in] y (const float&) The y position of the viewport
    * @param[in] w (const float&) The width of the viewport
    * @param[in] h (const float&) The height of the viewport
    */
    void SetViewport(const float& x, const float& y, const float& w, const float& h);

    /**
    * @brief Adds a multi render target to this render target
    * @param[in] snuffbox::D3D11RenderTarget* The target to add
    */
    void AddMultiTarget(D3D11RenderTarget* multi);

    /**
    * @brief Sets the shader resources of this render target and multi targets
    * @param[in] ID3D11DeviceContext* The immediate context to set the shader resources from
    * @param[in] ID3D11ShaderResourceView* The depth stencil view resource used for deferred rendering
    */
    void SetResources(ID3D11DeviceContext* context, ID3D11ShaderResourceView* depth_stencil_resource);

    /**
    * @brief Finds and removes a render element when destructed
    * @param[in] ptr (snuffbox:D3D11RenderElement*) The element to remove
    */
    void FindAndRemove(D3D11RenderElement* ptr);

		/// Releases all referenced D3D11 com objects
		void Release();

		/**
		* @return const std::string& The name of this render target
		*/
		const std::string& name() const;

    /**
    * @return ID3D11ShaderResourceView* The texture resource of a normal render target
    */
    ID3D11ShaderResourceView* resource();

    /**
    * @return ID3D11RenderTargetView* The view of this render target
    */
    ID3D11RenderTargetView* view();

    /**
    * @return snuffbox::D3D11RenderQueue* The render queue of this render target
    */
    D3D11RenderQueue* queue();

		/**
		* @return snuffbox::D3D11Effect* The post-processing effect of this render target
		*/
		D3D11Effect* post_processing();

		/**
		* @return const std::string& The technique currently in use by this render target
		*/
		const std::string& technique();

		/**
		* @return snuffbox::D3D11Uniforms* The uniforms buffer of this render target
		*/
		D3D11Uniforms* uniforms();

    /**
    * @return snuffbox::D3D11Viewport* The viewport to be overrided from JavaScript
    */
    D3D11Viewport* viewport();

		/**
		* @return const bool& Should the depth buffer be cleared when rendering this target?
		*/
		const bool& clear_depth() const;

		/**
		* @return const bool& Should lighting be enabled when rendering this target?
		*/
		const bool& lighting_enabled() const;

		/**
		* @brief Sets the post processing effect of this render target
		* @param[in] path (const std::string&) The path to the effect file
		*/
		void set_post_processing(const std::string& path);

		/**
		* @brief Sets the post processing effect of this render target
		* @param[in] effect (snuffbox::D3D11Effect*) The pointer to the effect to set
		*/
		void set_post_processing(D3D11Effect* effect);

		/**
		* @brief Sets the technique of this render target
		* @param[in] technique (const std::string&) The technique to set
		*/
		void set_technique(const std::string& technique);

		/**
		* @brief Sets if the depth buffer should be cleared when rendering this target
		* @param[in] v (const bool&) The boolean value
		*/
		void set_clear_depth(const bool& v);

		/**
		* @brief Should lighting be enabled for this render target?
		* @param[in] v (const bool&) The boolean value
		*/
		void set_lighting_enabled(const bool& v);

		/// Default destructor
		virtual ~D3D11RenderTarget();

	private:
		bool valid_; //!< Is this render target valid?
		std::string name_; //!< The name of this render target
		RenderTargets type_; //!< The type of this render target
		ID3D11Texture2D* buffer_; //!< The buffer of this render target
		ID3D11RenderTargetView* view_; //!< The actual view of this render target
		ID3D11ShaderResourceView* resource_; //!< When used as a normal render target, use this to store the texture
    SharedPtr<D3D11RenderQueue> queue_; //!< The queue of this render target
		SharedPtr<D3D11Uniforms> uniforms_; //!< The uniforms buffer
    SharedPtr<D3D11Viewport> viewport_; //!< The viewport that can be overrided from JavaScript
		D3D11Effect* post_processing_; //! The post processing effect for this render target
		std::string technique_; //!< The technique of this render target
    std::vector<D3D11RenderTarget*> mrts_; //!< Multiple render targets
		bool clear_depth_; //!< Should the depth buffer be cleared for this render target?
		bool lighting_enabled_; //!< Should lighting be enabled when rendering this render target?
    int width_; //!< The width of this target
    int height_; //!< The height of this target

	public:
		JS_NAME("RenderTarget");
		static void RegisterJS(JS_CONSTRUCTABLE obj);
		static void JSClear(JS_ARGS args);
		static void JSSetPostProcessing(JS_ARGS args);
		static void JSSetTechnique(JS_ARGS args);
		static void JSSetUniform(JS_ARGS args);
    static void JSSetViewport(JS_ARGS args);
    static void JSAddMultiTarget(JS_ARGS args);
		static void JSSetClearDepth(JS_ARGS args);
		static void JSClearDepth(JS_ARGS args);
		static void JSSetLightingEnabled(JS_ARGS args);
		static void JSLightingEnabled(JS_ARGS args);
	};
}