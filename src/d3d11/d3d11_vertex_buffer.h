#pragma once

#include "../d3d11/d3d11_render_device.h"
#include <vector>

namespace snuffbox
{
  /**
  * @struct snuffbox::Vertex
  * @brief Stores vertex information for use with a vertex buffer
  * @author Dani�l Konings
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
			bitangent(XMFLOAT3(0.0f, 0.0f, 0.0f)),
			material_id(-1)
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
			bitangent(XMFLOAT3(0.0f, 0.0f, 0.0f)),
			material_id(-1)
		{}

    XMFLOAT4 position; //!< The position of the vertex
    XMFLOAT4 colour; //!< The colour of the vertex
    XMFLOAT2 tex_coords; //!< The texture coordinates of the vertex
    XMFLOAT3 normal; //!< The normal of the vertex
    XMFLOAT3 tangent; //!< The tangent of this vertex
		XMFLOAT3 bitangent; //!< The bitangent of this vertex
		int material_id; //!< The material ID of this vertex, only used in models

    static const UINT STRIDE_SIZE = sizeof(XMFLOAT4) + sizeof(XMFLOAT4) + sizeof(XMFLOAT2) + sizeof(XMFLOAT3) + sizeof(XMFLOAT3) + sizeof(XMFLOAT3) + sizeof(int);
  };

  /**
  * @class snuffbox::D3D11VertexBuffer
  * @brief Contains vertex, index and topology information
  * @author Dani�l Konings
  */
  class D3D11VertexBuffer
  {
  public:
    /**
    * @enum snuffbox::VertexBufferType
    * @brief Used to give vertex buffers a type so the render device knows when to switch them
    * @author Dani�l Konings
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
		* @param[in] tangents (const bool&) Should tangents be calculated? Default = true
    */
		void Create(const std::vector<Vertex>& verts, const std::vector<int>& indices, const bool& tangents = true);

		/**
		* @brief Updates the vertex/index buffers without creating a new vertex buffer
		* @param[in] verts (const std::vector<snuffbox::Vertex>&) The vertices to set
		* @param[in] indices (const std::vector<int>&) The indices to set
		* @param[in] tangents (const bool&) Should tangents be calculated? Default = true
		*/
		void Update(const std::vector<Vertex>& verts, const std::vector<int>& indices, const bool& tangents = true);

    /// Calculates the bounds of the current vertices
    void CalculateBounds();

    /// Calculates the tangents of the current vertices
    void CalculateTangents();

    /// Sets the vertex/index buffer
    void Set();

    /** 
    * @brief Draws the set vertex/index buffer
    * @param[in] start (const int&) The index to start drawing from, default = 0
    */
    void Draw(const int& start = 0);

    /**
    * @brief Preforms a picking test with a given ray on the vertices of this vertex buffer
    * @param[in] origin (const XMFLOAT3&) The origin of the ray
    * @param[in] dir (const XMFLOAT3&) The direction of the ray
    * @param[in] world (const XMMATRIX&) The world matrix to transform with
    * @param[out] distance (float*) The distance the ray hit at
    * @return bool Was there an intersection at all?
    */
    bool Pick(const XMFLOAT3& origin, const XMFLOAT3& dir, float* distance, const XMMATRIX& world);

    /**
    * @return const snuffbox::D3D11VertexBuffer::VertexBufferType& The type of the vertex buffer
    */
    const VertexBufferType& type() const;

    /**
    * @return const D3D11_PRIMITIVE_TOPOLOGY& The topology this vertex buffer uses
    */
    const D3D11_PRIMITIVE_TOPOLOGY& topology() const;

    /**
    * @return const BoundingBox& The bounds of the vertices
    */
    const BoundingBox& bounds() const;

    /**
    * @brief Sets the primitive topology this vertex buffer uses
    * @param[in] topology (const D3D11_PRIMITIVE_TOPOLOGY&) The topology to use
    */
    void set_topology(const D3D11_PRIMITIVE_TOPOLOGY& topology);

		/**
		* @brief Sets the number of indices to draw
		* @param[in] n (const int&) The new value, set to < 0 to disable the override
		*/
		void set_num_indices(const int& n);

  private:
    std::vector<Vertex> vertices_; //!< The vertices of this vertex buffer
    std::vector<int> indices_; //!< The indices of this vertex buffer
    VertexBufferType type_; //!< The type of the vertex buffer
    D3D11_PRIMITIVE_TOPOLOGY topology_; //!< The tolopogy this vertex buffer uses
    ID3D11Buffer* vertex_buffer_; //!< The actual vertex buffer
    ID3D11Buffer* index_buffer_; //!< The actual index buffer
		unsigned int vertex_size_; //!< The number of vertices after creation
		unsigned int index_size_; //!< The number of indices after creation
    bool valid_; //!< Is this vertex buffer valid and ready for use?
    BoundingBox bounds_; //!< The boundaries of the vertices
		int num_indices_; //!< The number of indices to draw, defaults to -1, overrideable by setting to a number > 0
  };
}