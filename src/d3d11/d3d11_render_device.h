#pragma once

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcommon.h>
#include <D3DX10.h>
#include <D3DX11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

using namespace DirectX;

#include <string>
#include <vector>

#include "../platform/platform_render_device_base.h"
#include "../memory/shared_ptr.h"

#define SNUFF_SAFE_RELEASE(ptr, ctx) SNUFF_ASSERT_NOTNULL(ptr, ctx) ptr->Release(); ptr = nullptr;

namespace snuffbox
{
	class D3D11RenderTarget;
  class D3D11VertexBuffer;
  class D3D11InputLayout;
  class D3D11Viewport;
  class D3D11SamplerState;
	class D3D11ConstantBuffer;
	class D3D11Camera;
  class D3D11BlendState;
	class D3D11DepthState;
	class D3D11Texture;
	class D3D11Effect;
	class D3D11Shader;
	class D3D11Lighting;
  class D3D11Material;
	class D3D11Line;
	class D3D11RasterizerState;

	/**
	* @class snuffbox::D3D11RenderDevice
	* @brief A Direct3D 11 render device to display graphics with
	* @author Daniël Konings
	*/
	class D3D11RenderDevice : public IRenderDeviceBase
	{
  public:
    /**
    * @struct D3D11RenderDevice::RenderCommand
    * @brief Used to render a given target with a given camera
    * @author Daniël Konings
    */
    struct RenderCommand
    {
      D3D11Camera* camera = nullptr;
      D3D11RenderTarget* target = nullptr;
    };

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
		* @param[in] hr (const HRESULT&) The HRESULT to convert
		* @param[in] context (const std::string&) The context this error occured in
		* @return std::string The converted string
		*/
		std::string HRToString(const HRESULT& hr, const std::string& context);

		/// @see snuffbox::IRenderDeviceBase::Initialise
		bool Initialise();

		/// Creates the device
		void CreateDevice();

    /// Creates the backbuffer
    void CreateBackBuffer();

    /// Creates the screen quad
    void CreateScreenQuad();

    /// Loads the base shaders
    void LoadBaseShaders();

    /// Creates the input layout
    void CreateInputLayout();

    /// Creates the base viewport
    void CreateBaseViewport();

    /// Creates the default material
    void CreateDefaultMaterial();

    /// Creates the depth stencil view
		void CreateDepthStencilView();

		/// Finds the default adapter to use
		void FindAdapter();

    /**
    * @brief Receives a render command
    * @param[in] command (const snuffbox::D3D11Renderdevice::RenderCommand&) The command to receive
    */
    void ReceiveCommand(const RenderCommand& command);

    /// @see snuffbox::IRenderDeviceBase::StartDraw
    void StartDraw();

		/// @see snuffbox::IRenderDeviceBase::Draw
		void Draw();

    /**
    * @brief Draws a given render target
    * @param[in] target (snuffbox::D3D11RenderTarget*) The render target to draw
    */
    void DrawRenderTarget(D3D11RenderTarget* target);

		/// @see snuffbox::IRenderDeviceBase::ResizeBuffers
		void ResizeBuffers(const int& w, const int& h);

		/// @see snuffbox::IRenderDeviceBase::Dispose
		void Dispose();

		/**
		* @brief Adds a render target to the map of render targets
		* @param[in] target (snuffbox::D3D11RenderTarget*) The render target to add
		*/
    void AddRenderTarget(D3D11RenderTarget* target);

		/// Maps the UI constant buffer
		void MapUIBuffer();

		/// Maps the global constant buffer
		void MapGlobalBuffer();

		/**
		* @brief Sets the sampler currently in use
		* @param[in] type (const int&) The type of sampler to set, castable to snuffbox::D3D11SamplerState::SamplerTypes
		*/
		void SetSampler(const int& type);

    /**
    * @brief Sets the fullscreen state of the render device
    * @param[in] fullscreen (const bool&) The boolean value
    */
    void SetFullscreen(const bool& fullscreen);

    /**
    * @brief Retrieves a render target by name
    * @param[in] name (const std::string&) The name of the render target to retrieve
    */
    D3D11RenderTarget* GetTarget(const std::string& name);

    /**
    * @brief Removes a given render target from the device
    * @param[in] target (snuffbox::D3D11RenderTarget*) The target to remove
    */
    void RemoveTarget(D3D11RenderTarget* target);

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
		* @return snuffbox::D3D11ConstantBuffer* The global constant buffer
		*/
		D3D11ConstantBuffer* constant_buffer();

		/**
		* @return snuffbox::D3D11Camera* The camera currently in use
		*/
		D3D11Camera* camera();

		/**
		* @return snuffbox::D3D11Texture** The currently set textures
		*/
		D3D11Texture** set_textures();

		/**
		* @return snuffbox::D3D11Shader* The currently set shader
		*/
		D3D11Shader* current_shader();

    /**
    * @return snuffbox::D3D11VertexBuffer* The currently set model
    */
    D3D11VertexBuffer* current_model();

    /**
    * @return snuffbox::D3D11BlendState* The currently set blend state
    */
    D3D11BlendState* current_blend_state();

    /**
    * @return snuffbox::D3D11DepthState* The currently set depth state
    */
    D3D11DepthState* current_depth_state();

		/**
		* @return snuffbox::D3D11DepthState* The currently set rasterizer state
		*/
		D3D11RasterizerState* current_rasterizer_state();

		/**
		* @return snuffbox::D3D11RenderTarget* The current target being rendered to
		*/
		D3D11RenderTarget* current_target();

    /**
    * @return snuffbox::D3D11Texture* The default texture
    */
    D3D11Texture* default_texture();

    /**
    * @return snuffbox::D3D11Texture* The default texture
    */
    D3D11Texture* default_normal();

    /**
    * @return snuffbox::D3D11Texture* The default cube map
    */
    D3D11Texture* default_cube_map();

    /**
    * @return snuffbox::D3D11Texture* The default effect
    */
    D3D11Effect* default_effect();

		/**
		* @return snuffbox::D3D11Texture* The default post processing effect
		*/
		D3D11Effect* default_post_processing();

    /**
    * @return snuffbox::D3D11Texture* The default material
    */
    D3D11Material* default_material();

    /**
    * @return snuffbox::D3D11BlendState* The default depth state
    */
    D3D11DepthState* default_depth_state();

    /**
    * @return snuffbox::D3D11BlendState* The default blend state
    */
    D3D11BlendState* default_blend_state();

		/**
		* @return snuffbox::D3D11RasterizerState* The default rasterizer state
		*/
		D3D11RasterizerState* default_rasterizer_state();

		/**
		* @return snuffbox::D3D11Viewport* The viewport a render target uses
		*/
		D3D11Viewport* viewport_render_target();

		/**
		* @return snuffbox::D3D11Viewport* The viewport the full screen pass uses
		*/
		D3D11Viewport* viewport();

		/**
		* @return ID3D11ShaderResourceView* The depth stencil resource of the depth stencil view
		*/
		ID3D11ShaderResourceView* depth_stencil_resource();

    /**
    * @brief Sets the current vertex buffer type
    * @param[in] type (const int&) The type to set
    */
    void set_vertex_buffer_type(const int& type);

		/**
		* @brief Sets the camera to render with
		* @param[in] camera (snuffbox::D3D11Camera*) The pointer to the camera
		*/
		void set_camera(D3D11Camera* camera);

		/**
		* @brief Sets the current shader
		* @param[in] shader (snuffbox::D3D11Shader*) The shader to set
		*/
		void set_current_shader(D3D11Shader* shader);

    /**
    * @brief Sets the vertex buffer of a model as the current vertex buffer
    * @param[in] model (snuffbox::D3D11VertexBuffer*) The model to set
    */
    void set_current_model(D3D11VertexBuffer* model);

    /**
    * @brief Sets the current depth state
    * @param[in] state (snuffbox::D3D11DepthState*) The state to set
    */
    void set_current_depth_state(D3D11DepthState* state);

    /**
    * @brief Sets the current blend state
    * @param[in] state (snuffbox::D3D11BlendState*) The state to set
    */
    void set_current_blend_state(D3D11BlendState* state);

		/**
		* @brief Sets the current rasterizer state
		* @param[in] state (snuffbox::D3D11RasterizerState*) The state to set
		*/
		void set_current_rasterizer_state(D3D11RasterizerState* state);

		/// Default destructor
		virtual ~D3D11RenderDevice();

	private:
		bool ready_; //!< Is the device ready?

		IDXGIAdapter* adapter_; //!< The adapter that will be used for rendering
		IDXGISwapChain* swap_chain_; //!< The swap chain this device will be created with
		ID3D11Device* device_; //!< The actual device
		ID3D11DeviceContext* context_; //!< The device context

		SharedPtr<D3D11RenderTarget> back_buffer_; //!< The backbuffer of this render device
		std::vector<D3D11RenderTarget*> render_targets_; //!< The map of render targets
    std::vector<RenderCommand> commands_; //!< The commands for drawing
		D3D11RenderTarget* current_target_; //!< The current target being rendered

    SharedPtr<D3D11VertexBuffer> screen_quad_; //!< The vertex buffer of the screen quad
    int vertex_buffer_type_; //!< The current vertex buffer type

    SharedPtr<D3D11InputLayout> input_layout_; //!< The input layout

    SharedPtr<D3D11Viewport> viewport_; //!< The base viewport
    SharedPtr<D3D11Viewport> viewport_render_target_; //!< The render target viewport

    SharedPtr<D3D11SamplerState> sampler_linear_; //!< The linear sampler state
		SharedPtr<D3D11SamplerState> sampler_anisotropic_; //!< The anisotropic sampler state
    SharedPtr<D3D11SamplerState> sampler_point_; //!< The point sampler state

		D3D11Camera* camera_; //!< The current camera

		D3D11ConstantBuffer* constant_buffer_; //!< The global constant buffer

    SharedPtr<D3D11BlendState> default_blend_state_; //!< The default blend state
		SharedPtr<D3D11DepthState> default_depth_state_; //!< The default depth state
		SharedPtr<D3D11RasterizerState> default_rasterizer_state_; //!< The default rasterizer state

		ID3D11DepthStencilView* depth_stencil_view_; //!< The depth stencil view
		ID3D11Texture2D* depth_stencil_buffer_; //!< The actual depth stencil texture
    ID3D11ShaderResourceView* depth_stencil_resource_; //!< The actual depth stencil resource

		D3D11Texture* set_textures_[8]; //!< The currently set textures
		D3D11Shader* current_shader_; //!< The currently set shaders
    D3D11VertexBuffer* current_model_; //!< The currently set model
    D3D11BlendState* current_blend_state_; //!< The currently set blend state
    D3D11DepthState* current_depth_state_; //!< The currently set depth state
		D3D11RasterizerState* current_rasterizer_state_; //!< The currently set rasterizer state

		D3D11Lighting* lighting_; //!< The lighting system

    SharedPtr<D3D11Texture> default_texture_; //!< The default texture
    SharedPtr<D3D11Texture> default_normal_; //!< The default texture
    SharedPtr<D3D11Texture> default_cube_map_; //!< The default cube map
    SharedPtr<D3D11Effect> default_effect_; //!< The default effect
		SharedPtr<D3D11Effect> default_post_processing_; //!< The default effect
    SharedPtr<D3D11Material> default_material_; //!< The default material
	};
}