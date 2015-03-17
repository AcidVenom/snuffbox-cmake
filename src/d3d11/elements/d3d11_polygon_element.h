#pragma once

#include "../../d3d11/elements/d3d11_render_element.h"
#include "../../d3d11/d3d11_vertex_buffer.h"

namespace snuffbox
{
  /**
  * @class snuffbox::D3D11Polygon
  * @brief A polygon to add vertices and indices to yourself
  * @author Daniël Konings
  */
  class D3D11Polygon : public D3D11RenderElement, public JSObject
  {
  public:
    /// Default constructor
    D3D11Polygon();

    /// JavaScript constructor
    D3D11Polygon(JS_ARGS args);

    /**
    * @brief Adds a vertex with a given description
    * @param[in] position (const XMFLOAT4&) The position of the vertex
    * @param[in] colour (const XMFLOAT4&) The colour of the vertex
    * @param[in] texcoord (const XMFLOAT2&) The texture coordinates of the vertex
    * @param[in] normal (const XMFLOAT3&) The normal of the vertex
    */
    void AddVertex(const XMFLOAT4& position, const XMFLOAT4& colour, const XMFLOAT2& texcoord, const XMFLOAT3& normal, const XMFLOAT3& tangent, const XMFLOAT3& bitangent);

    /**
    * @brief Removes a vertex at a given index
    * @param[in] idx (const int&) The index of the vertex to remove
    */
    void RemoveVertex(const int& idx);

    /**
    * @brief Edits a vertex with a given description at a given index
    * @param[in] idx (const int&) The index of the vertex to edit
    * @param[in] position (const XMFLOAT4&) The position of the vertex
    * @param[in] colour (const XMFLOAT4&) The colour of the vertex
    * @param[in] texcoord (const XMFLOAT2&) The texture coordinates of the vertex
    * @param[in] normal (const XMFLOAT3&) The normal of the vertex
    */
    void SetVertex(const int& idx, const XMFLOAT4& position, const XMFLOAT4& colour, const XMFLOAT2& texcoords, const XMFLOAT3& normal, const XMFLOAT3& tangent, const XMFLOAT3& bitangent);

    /// Clears all vertices
    void ClearVertices();

    /**
    * @return int The number of vertices currently in the index vector
    */
    int NumVertices();

    /**
    * @brief Adds an index
    * @param[in] index (const int&) The index to add
    */
    void AddIndex(const int& index);

    /**
    * @brief Removes an index at a given index
    * @param[in] idx (const int&) The index of the index to remove
    */
    void RemoveIndex(const int& idx);

    /**
    * @brief Edits an index at a given index
    * @param[in] idx (const int&) The index of the index to edit
    * @param[in] index (const int&) The value to change the index to
    */
    void SetIndex(const int& idx, const int& index);

    /// Clears all indices
    void ClearIndices();

    /**
    * @return int The number of indices currently in the index vector
    */
    int NumIndices();

    /**
    * @brief Flushes the vertex buffer
    * @param[in] tangents (const bool&) Should tangents be calculated for this polygon? Default = false
    */
    void Flush(const bool& tangents = false);

    /**
    * @brief Sets the primitive topology of this polygon
    * @param[in] topology (const int&) The primitive topology to set
    */
    void set_topology(const int& topology);

    /// @see snuffbox::D3D11RenderElement
    D3D11VertexBuffer* vertex_buffer();

    /// Default destructor
    virtual ~D3D11Polygon();

  private:
    SharedPtr<D3D11VertexBuffer> vertex_buffer_; //!< The vertex buffer of this quad
    std::vector<Vertex> vertices_; //!< The vertices of this polygon
    std::vector<int> indices_; //!< The indices of this polygon
    int topology_; //!< The topology of this polygon element

  public:
    JS_NAME("Polygon");
    static void RegisterJS(JS_CONSTRUCTABLE obj);
    static void JSAddVertex(JS_ARGS args);
    static void JSSetVertex(JS_ARGS args);
    static void JSRemoveVertex(JS_ARGS args);
    static void JSClearVertices(JS_ARGS args);
    static void JSNumVertices(JS_ARGS args);
    static void JSAddIndex(JS_ARGS args);
    static void JSSetIndex(JS_ARGS args);
    static void JSRemoveIndex(JS_ARGS args);
    static void JSClearIndices(JS_ARGS args);
    static void JSNumIndices(JS_ARGS args);
    static void JSFlush(JS_ARGS args);
    static void JSSetTopology(JS_ARGS args);
    static void JSSetBillboarding(JS_ARGS args);
  };
}