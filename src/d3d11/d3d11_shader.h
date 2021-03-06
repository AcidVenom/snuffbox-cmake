#pragma once

#include "../content/content.h"
#include "../d3d11/d3d11_render_device.h"

namespace snuffbox
{
  /**
  * @class snuffbox::D3D11Shader
  * @brief A Direct3D 11 shader used to draw vertex buffers to the screen
  * @author Dani�l Konings
  */
  class D3D11Shader : public Content
  {
  public:
    /// Default constructor
    D3D11Shader();

    /**
    * @brief Loads a shader from a path
    * @param[in] path (const std::string&) The path to load from
    */
    void Load(const std::string& path);

    /// Sets the vertex/pixel shader
    void Set();

    /**
    * @return ID3D10Blob* The vertex shader buffer
    */
    ID3D10Blob* vs_buffer();

    /**
    * @return ID3D10Blob* The pixel shader buffer
    */
    ID3D10Blob* ps_buffer();

		/**
		* @return const bool& Is this shader valid?
		*/
		const bool& valid() const;

    /// Default destructor
    virtual ~D3D11Shader();

  private:
    ID3D10Blob* vs_buffer_; //!< The vertex shader buffer
    ID3D10Blob* ps_buffer_; //!< The pixel shader buffer
    ID3D11VertexShader* vs_; //!< The vertex shader
    ID3D11PixelShader* ps_; //!< The pixel shader
    bool valid_; //!< Is the shader valid?
  };
}