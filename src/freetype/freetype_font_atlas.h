#pragma once

#include "../d3d11/d3d11_render_device.h"
#include <vector>

namespace snuffbox
{
  /**
  * @struct snuffbox::FontAtlasNode
  * @brief Basically just contains an int3
  * @author Daniël Konings
  */
  struct FontAtlasNode
  {
    FontAtlasNode() : x(0), y(0), z(0){}
    FontAtlasNode(int xx, int yy, int zz) : x(xx), y(yy), z(zz){}
    int x, y, z;
  };

  /**
  * @struct snuffbox::FontAtlasRegion
  * @brief Contains information for a region, X, Y, width and height
  * @author Daniël Konings
  */
  struct FontAtlasRegion
  {
    FontAtlasRegion(int xx, int yy, int w, int h) : x(xx), y(yy), width(w), height(h) {}
    int width, height;
    int x, y;
  };

  /**
  * @class snuffbox::FontAtlas
  * @brief A font atlas that contains pixel information provided by snuffbox::Font
  * @author Daniël Konings
  */
  class FontAtlas
  {
  public:
    /**
    * @brief Construct a font atlas with a size
    * @param[in] size (const int&) The size of the atlas
    * @param[in] depth (const unsigned char&) The depth of the atlas
    */
		FontAtlas(const int& size, const unsigned char& depth);

		/**
		* @brief Creates a font atlas region with a given width and height
		* @param[in] width (int) The width of the new region
		* @param[in] height (int) The height of the new region
		*/
		FontAtlasRegion CreateRegion(const int& width, const int& height);

		/**
		* @brief Tests if a region fits
		* @param[in] idx (int) The index to check from
		* @param[in] width (const int&) The width to check
		* @param[in] height (const int&) The height to check
		* @return (int) The best index found to put a region in
		*/
		int TestFit(int idx, const int& width, const int& height);

		/// Merges all layers of the atlas
		void Merge();

		/**
		* @brief Fills a region with data
		* @param[in] region (const snuffbox::FontAtlasRegion&) The region to fill
		* @param[in] data (const unsigned char*) The data to fill the region with
		* @param[in] stride (const int&) The stride of the data
		*/
		void FillRegion(const FontAtlasRegion& region, const unsigned char* data, const int& stride);

		/**
		* @brief Clears a region in the atlas
		* @param[in] region (const snuffbox::FontAtlasRegion&) The region to clear
		*/
		void ClearRegion(const FontAtlasRegion& region);

		/// Clears the entire atlas
		void Clear();

		/// Creates the texture
		void CreateTexture();

		/**
		* @return D3D11Texture* Retrieves the texture of this atlas
		*/
		D3D11Texture* texture();

		/**
		* @return const int& The size of this atlas
		*/
		const int& size() const;

		/**
		* @return const unsigned char& The depth of this atlas
		*/
		const unsigned char& depth() const;

    /// Default destructor
    ~FontAtlas();

  private:
		SharedPtr<D3D11Texture>			texture_; //!< The actual texture of the atlas
    int													size_; //!< The size of the atlas for both width and height
    std::vector<unsigned char>	data_; //!< The currently contained data
    std::vector<FontAtlasNode>	nodes_; //!< The nodes of this font atlas
		unsigned char								depth_; //!< The depth of this atlas
    int													used_; //!< The number of used slots
  };
}