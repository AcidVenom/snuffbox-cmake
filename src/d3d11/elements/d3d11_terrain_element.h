#pragma once

#include "../../d3d11/elements/d3d11_render_element.h"
#include <vector>

namespace snuffbox
{
	struct Vertex;
  /**
  * @class snuffbox::D3D11Terrain
  * @brief A terrain element made up out of quads
  * @author Daniël Konings
  */
  class D3D11Terrain : public D3D11RenderElement, public JSObject
  {
	public:

		/**
		* @struct snuffbox::D3D11Terrain::Indices
		* @brief Basically an int2
		* @author Daniël Konings
		*/
		struct Indices
		{
			int x, y;
		};

		/**
		* @struct snuffbox::D3D11Terrain::WorldCoordinates
		* @brief Basically a float2
		* @author Daniël Konings
		*/
		struct WorldCoordinates
		{
			float x, z;
		};

  public:
    /// Default constructor
		D3D11Terrain();

    /// JavaScript constructor
		D3D11Terrain(JS_ARGS args);

    /**
		* @brief Creates the vertex buffer by dimensions
		* @param[in] w (const int&) The width of the terrain
		* @param[in] h (const int&) The height of the terrain
		*/
		void Create(const int& w, const int& h);

		/**
		* @brief Maps a world coordinate to indices
		* @param[in] x (const float&) The x position
		* @param[in] y (const float&) The y position
		* @return snuffbox::D3D11Terrain::Indices The converted indices
		*/
		Indices WorldToIndex(const float& x, const float& y);

		/**
		* @brief Maps indices to world coordinates
		* @param[in] x (const int&) The x index
		* @param[in] y (const int&) The y index
		* @return snuffbox::D3D11Terrain::WorldCoordinates The converted coordinates
		*/
		WorldCoordinates IndexToWorld(const int& x, const int& y);

		/**
		* @brief Retrieves the 3 nearest vertices of a world position
		* @param[in] x (const float&) The x position
		* @param[in] y (const float&) The y position
		* @return std::vector<snuffbox::D3D11Terrain::Indices> The nearest vertices
		*/
		std::vector<Indices> NearestVertices(const float& x, const float& y);

		/**
		* @brief Sets the height of a vertex by index
		* @param[in] x (const int&) The x index of the vertex
		* @param[in] y (const int&) The y index of the vertex
		* @param[in] h (const float&) The height to set
		*/
		void SetHeight(const int& x, const int& y, const float& h);

		/**
		* @brief Retrieves the height of a vertex by index
		* @param[in] x (const int&) The x index of the vertex
		* @param[in] y (const int&) The y index of the vertex
		* @return float The height of the vertex
		*/
		float GetHeight(const int& x, const int& y);

		/**
		* @brief Sets the normals of a vertex
		* @param[in] x (const int&) The x index of the vertex
		* @param[in] y (const int&) The y index of the vertex
		*/
		void SetNormals(const int& x, const int& y);

		/**
		* @brief Calculates the normals of a triangle
		* @param[in] index_a (int) The index of the first vertex of the triangle
		* @param[in] index_b (int) The index of the second vertex of the triangle
		* @param[in] index_c (int) The index of the third vertex of the triangle
		*/
		void CalculateNormals(const int& index_a, const int& index_b, const int& index_c);

		/// Flushes the terrain after modification of heights
		void Flush();

    /// @see snuffbox::D3D11RenderElement
    D3D11VertexBuffer* vertex_buffer();

		/**
		* @return const int& The width of the current terrain
		*/
		const int& width() const;

		/**
		* @return const int& The height of the current terrain
		*/
		const int& height() const;

    /// Default destructor
		virtual ~D3D11Terrain();

  private:
    SharedPtr<D3D11VertexBuffer> vertex_buffer_; //!< The vertex buffer of this quad
		std::vector<Vertex> vertices_; //!< The vertices of this terrain
		std::vector<int> indices_; //!< The indices of this terrain
		int width_; //!< The width of the terrain
		int height_; //!< The height of the terrain

  public:
    JS_NAME("Terrain");
    static void RegisterJS(JS_CONSTRUCTABLE obj);
		static void JSCreate(JS_ARGS args);
		static void JSWidth(JS_ARGS args);
		static void JSHeight(JS_ARGS args);
		static void JSWorldToIndex(JS_ARGS args);
		static void JSIndexToWorld(JS_ARGS args);
		static void JSNearestVertices(JS_ARGS args);
		static void JSSetHeight(JS_ARGS args);
		static void JSGetHeight(JS_ARGS args);
		static void JSFlush(JS_ARGS args);
  };
}