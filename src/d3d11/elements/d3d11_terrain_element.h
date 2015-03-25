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
		* @brief Basically a float3
		* @author Daniël Konings
		*/
		struct WorldCoordinates
		{
			float x, y, z;
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

    /// Prepares the textures for use
    void PrepareTextures();

		/**
		* @brief Maps a world coordinate to indices
		* @param[in] x (const float&) The x position
		* @param[in] y (const float&) The y position
    * @param[out] in_bounds_x (bool*) Is the x coordinate in bounds?
    * @param[out] in_bounds_y (bool*) Is the y coordinate in bounds?
		* @return snuffbox::D3D11Terrain::Indices The converted indices
		*/
    Indices WorldToIndex(const float& x, const float& y, bool* in_bounds_x, bool* in_bounds_y);

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
		* @brief Calculates the normals of 3 given vertices
		* @param[in] a (const int&) The index of the first vertex
		* @param[in] b (const int&) The index of the second vertex
		* @param[in] c (const int&) The index of the third vertex
		*/
		void CalculateNormals(const int& a, const int& b, const int& c);

    /**
    * @brief Calculates and returns the texture coordinates relative to the terrain of a world position
    * @param[in] x (const float&) The world x position
    * @param[in] y (const float&) The world y position
    * @return XMFLOAT2 The texture coordinates at the position
    */
    XMFLOAT2 GetWorldTextureCoordinates(const float& x, const float& y);

    /**
    * @brief Retrieves the bilinear height of a given position
    * @param[in] x (const float&) The world x position to calculate the height from
    * @param[in] y (const float&) The world y position to calculate the height from
    * @return float The bilinear interpolated height of the position
    */
    float GetBilinearHeight(const float& x, const float& y);

    /**
    * @brief Brushes a texture onto the terrain with a given brush texture
    * @param[in] brush (const std::string&) The texture mask for the brush
    * @param[in] texture (const std::string&) The texture to brush onto the terrain
    * @param[in] x (const float&) The world x coordinate to brush on
    * @param[in] y (const float&) The world y coordinate to brush on
    * @param[in] radius (const float&) The radius of the brush
		* @param[in] opacity (const float&) The opacity of the brush, default = 1.0f
    * @param[in] normal (const std::string&) The normal texture for the brush, optional
    * @param[in] specular (const std::string&) The specular texture for the brush, optional
    */
		void BrushTexture(const std::string& brush, const std::string& texture, const float& x, const float& y, const float& radius, const float& opacity = 1.0f, const std::string& normal = "null", const std::string& specular = "null");

		/// Flushes the terrain after modification of heights
		void Flush();

		/**
		* @brief Saves the terrain texture to a path
		* @param[in] path (const std::string&) The path to save to
		*/
		void SaveTexture(const std::string& path);

		/**
		* @brief Loads a terrain texture as a base layer
		* @param[in] path (const std::string&) The path to load from
		*/
		void LoadTexture(const std::string& path);

    /**
    * @brief Sets the texture tiling of the terrain
    * @param[in] u (const float&) The amount of times the texture needs to tile on the x-axis
    * @param[in] v (const float&) The amount of times the texture needs to tile on the y-axis
    */
    void set_texture_tiling(const float& u, const float& v);

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
		SharedPtr<D3D11VertexBuffer> brush_vbo_; //!< The brush vertex buffer
    SharedPtr<D3D11RenderTarget> diffuses_; //!< The terrain diffuses render target that can be drawn to
    SharedPtr<D3D11RenderTarget> normals_; //!< The terrain normals render target that can be drawn to
    SharedPtr<D3D11RenderTarget> speculars_; //!< The terrain speculars render target that can be drawn to
    SharedPtr<D3D11Texture> diffuse_map_; //!< The diffuse map texture
    SharedPtr<D3D11Texture> normal_map_; //!< The normal map texture
    SharedPtr<D3D11Texture> specular_map_; //!< The specular map texture
    SharedPtr<D3D11Viewport> brush_vp_; //!< The viewport to brush the texture with
    SharedPtr<D3D11Material> material_; //!< The terrain material
    SharedPtr<D3D11Uniforms> brush_uniform_; //!< The brush uniform
    XMFLOAT2 texture_tiling_; //!< The texture tiling of this terrain
    D3D11Shader* brush_shader_; //!< The brush shader for this terrain to use
		std::vector<Vertex> vertices_; //!< The vertices of this terrain
		std::vector<int> indices_; //!< The indices of this terrain
		int width_; //!< The width of the terrain
		int height_; //!< The height of the terrain
    float texture_size_; //!< The default texture width of this terrain

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
    static void JSGetBilinearHeight(JS_ARGS args);
    static void JSBrushTexture(JS_ARGS args);
    static void JSSetTextureTiling(JS_ARGS args);
		static void JSFlush(JS_ARGS args);
		static void JSSaveTexture(JS_ARGS args);
		static void JSLoadTexture(JS_ARGS args);
  };
}