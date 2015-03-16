#pragma once

#include "../d3d11/d3d11_render_device.h"
#include <vector>

namespace snuffbox
{
  /**
  * @struct snuffbox::Vertex
  * @brief Stores vertex information for use with a vertex buffer
  * @author Daniël Konings
  */
  struct Vertex
  {
		/// Default constructor
		Vertex() :
			position(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)),
			colour(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)),
			tex_coords(XMFLOAT2(0.0f, 0.0f)),
			normal(XMFLOAT3(0.0f, 0.0f, 0.0f)),
			tangent(XMFLOAT3(0.0f, 0.0f, 0.0f)),
			bitangent(XMFLOAT3(0.0f, 0.0f, 0.0f))
		{}

		/**
		* @brief Construct a new vertex with given attributes
		* @param[in] p (const XMFLOAT4&) The position of the new vertex
		* @param[in] c (const XMFLOAT4&) The colour of the new vertex
		* @param[in] t (const XMFLOAT2&) The texture coordinates of the new vertex
		* @param[in] n (const XMFLOAT3&) The normal of the new vertex
		*/
		Vertex(const XMFLOAT4& p, const XMFLOAT4& c, const XMFLOAT2& t, const XMFLOAT3 n) :
			position(p),
			colour(c),
			tex_coords(t),
			normal(n),
			tangent(XMFLOAT3(0.0f, 0.0f, 0.0f)),
			bitangent(XMFLOAT3(0.0f, 0.0f, 0.0f))
		{}

    XMFLOAT4 position; //!< The position of the vertex
    XMFLOAT4 colour; //!< The colour of the vertex
    XMFLOAT2 tex_coords; //!< The texture coordinates of the vertex
    XMFLOAT3 normal; //!< The normal of the vertex
    XMFLOAT3 tangent; //!< The tangent of this vertex
		XMFLOAT3 bitangent; //!< The bitangent of this vertex

    static const UINT stride_size = sizeof(XMFLOAT4)+sizeof(XMFLOAT4)+sizeof(XMFLOAT2)+sizeof(XMFLOAT3)+sizeof(XMFLOAT3)+sizeof(XMFLOAT3);
  };

  /**
  * @class snuffbox::D3D11VertexBuffer
  * @brief Contains vertex, index and topology information
  * @author Daniël Konings
  */
  class D3D11VertexBuffer
  {
  public:
    /**
    * @enum snuffbox::VertexBufferType
    * @brief Used to give vertex buffers a type so the render device knows when to switch them
    * @author Daniël Konings
    */
    enum VertexBufferType : int
    {
      kScreen,
      kQuad,
      kOther
    };

  public:
    /**
    * @brief Constructs a vertex buffer with a type
    * @param[in] type (const snuffbox::D3D11VertexBuffer::VertexBufferType&) The type of this vertex buffer
    */
    D3D11VertexBuffer(const VertexBufferType& type);

    /// Default destructor
    ~D3D11VertexBuffer();

    /**
    * @brief Creates the vertex/index buffers for use
    * @param[in] verts (const std::vector<snuffbox::Vertex>&) The vertices to add
    * @param[in] indices (const std::vector<int>&) The indices to add
    */
    void Create(const std::vector<Vertex>& verts, const std::vector<int>& indices);

    /// Calculates the tangents of the the current vertices
    void CalculateTangents();

    /// Sets the vertex/index buffer
    void Set();

    /// Draws the set vertex/index buffer
    void Draw();

    /**
    * @return const snuffbox::D3D11VertexBuffer::VertexBufferType& The type of the vertex buffer
    */
    const VertexBufferType& type() const;

    /**
    * @return const D3D11_PRIMITIVE_TOPOLOGY& The topology this vertex buffer uses
    */
    const D3D11_PRIMITIVE_TOPOLOGY& topology() const;

    /**
    * @brief Sets the primitive topology this vertex buffer uses
    * @param[in] topology (const D3D11_PRIMITIVE_TOPOLOGY&) The topology to use
    */
    void set_topology(const D3D11_PRIMITIVE_TOPOLOGY& topology);

  private:
    std::vector<Vertex> vertices_; //!< The vertices of this vertex buffer
    std::vector<int> indices_; //!< The indices of this vertex buffer
    VertexBufferType type_; //!< The type of the vertex buffer
    D3D11_PRIMITIVE_TOPOLOGY topology_; //!< The tolopogy this vertex buffer uses
    ID3D11Buffer* vertex_buffer_; //!< The actual vertex buffer
    ID3D11Buffer* index_buffer_; //!< The actual index buffer
    bool valid_; //!< Is this vertex buffer valid and ready for use?
  };
}