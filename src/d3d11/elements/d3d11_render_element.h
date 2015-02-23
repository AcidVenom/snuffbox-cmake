#pragma once

#include "../../js/js_object.h"
#include "../../d3d11/d3d11_render_device.h"

namespace snuffbox
{
  class D3D11VertexBuffer;

  /**
  * @class snuffbox::D3D11RenderElement
  * @brief Contains information for the render device to use in drawing different elements
  * @author Daniël Konings
  */
  class D3D11RenderElement
  {
  public:
    /// Default constructor
    D3D11RenderElement();

    /// Default destructor
    ~D3D11RenderElement();

    /**
    * @brief Spawns this element on a given render target by name
    * @param[in] target (const std::string&) The name of the target to spawn the element on
    */
    void Spawn(const std::string& target);

    /// Destroys the render element
    void Destroy();

    /**
    * @return snuffbox::D3D11VertexBuffer* The vertex buffer associated with this render element
    */
    virtual D3D11VertexBuffer* vertex_buffer() = 0;

    /**
    * @return const XMVECTOR& The translation of this render element
    */
    const XMVECTOR& translation() const;

    /**
    * @return const XMVECTOR& The rotation of this render element
    */
    const XMVECTOR& rotation() const;

    /**
    * @return const XMVECTOR& The scale of this render element
    */
    const XMVECTOR& scale() const;

    /**
    * @return const XMVECTOR& The offet of this render element
    */
    const XMVECTOR& offset() const;

    /**
    * @return const XMVECTOR& The size of this render element
    */
    const XMVECTOR& size() const;

    /**
    * @return const XMMATRIX& The world matrix of this render element
    */
    const XMMATRIX& world_matrix();

    /**
    * @return const bool& Was this render element spawned?
    */
    const bool& spawned() const;

    /**
    * @brief Sets the translation of this render element
    * @param[in] x (float) The new x position
    * @param[in] y (float) The new y position
    * @param[in] z (float) The new z position
    */
    void set_translation(float x, float y, float z);

    /**
    * @brief Sets the rotation of this render element
    * @param[in] x (float) The new x rotation
    * @param[in] y (float) The new y rotation
    * @param[in] z (float) The new z rotation
    */
    void set_rotation(float x, float y, float z);

    /**
    * @brief Sets the scaling of this render element
    * @param[in] x (float) The new x scaling
    * @param[in] y (float) The new y scaling
    * @param[in] z (float) The new z scaling
    */
    void set_scale(float x, float y, float z);

    /**
    * @brief Sets the offset of this render element
    * @param[in] x (float) The new x offset
    * @param[in] y (float) The new y offset
    * @param[in] z (float) The new z offset
    */
    void set_offset(float x, float y, float z);

    /**
    * @brief Sets the sizing of this render element
    * @param[in] x (float) The new x sizing
    * @param[in] y (float) The new y sizing
    * @param[in] z (float) The new z sizing
    */
    void set_size(float x, float y, float z);

  private:
    XMVECTOR translation_; //!< The translation vector of this render element
    XMVECTOR rotation_; //!< The rotation vector of this render element
    XMVECTOR scale_; //!< The scaling vector of this render element
    XMVECTOR offset_; //!< The offset vector of this render element
    XMVECTOR size_; //!< The size vector of this render element
    XMMATRIX world_matrix_; //!< The world matrix of this render element
    bool spawned_; //!< Is this render element spawned?

  public:
    static void Register(JS_CONSTRUCTABLE obj);
    static void JSSetTranslation(JS_ARGS args);
    static void JSTranslation(JS_ARGS args);
    static void JSSetRotation(JS_ARGS args);
    static void JSRotation(JS_ARGS args);
    static void JSSetScale(JS_ARGS args);
    static void JSScale(JS_ARGS args);
    static void JSSetOffset(JS_ARGS args);
    static void JSOffset(JS_ARGS args);
    static void JSSetSize(JS_ARGS args);
    static void JSSize(JS_ARGS args);
    static void JSSpawn(JS_ARGS args);
    static void JSDestroy(JS_ARGS args);
  };
}