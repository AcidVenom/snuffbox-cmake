#pragma once

#include "../d3d11/d3d11_render_device.h"
#include "../content/content.h"

namespace snuffbox
{
	/**
	* @class snuffbox::D3D11Texture
	* @brief A D3D11 texture loaded from an image format
	* @author Dani�l Konings
	*/
	class D3D11Texture : public Content
	{
	public:
		/// Default constructor
		D3D11Texture();

		/// @see snuffbox::Content
		void Load(const std::string& path);

		/**
		* @brief Creates the actual texture by a buffer
		* @param[in] width (const int&) The width of the texture
		* @param[in] height (const int&) The height of the texture
		* @param[in] format (const DXGI_FORMAT&) The format of the texture
		* @param[in] buffer (const void*) The pixel data buffer
		* @param[in] stride (const unsigned short&) The stride size of the buffer
		*/
		void Create(const int& width, const int& height, const DXGI_FORMAT& format, const void* buffer, const unsigned short& stride);

		/**
		* @brief Creates the actual texture by a shader resource view
		* @param[in] texture (ID3D11ShaderResourceView*) The shader resource view to create this texture with
		*/
		void Create(ID3D11ShaderResourceView* texture);

		/**
		* @brief Sets the texture as a shader resource on a given slot
		* @param[in] slot (const int&) The slot to store the texture in
		*/
		void Set(const int& slot);

		/**
		* @return const int& The width of the texture
		*/
		const int& width() const;

		/**
		* @return const int& The height of the texture
		*/
		const int& height() const;

		/// Default destructor
		virtual ~D3D11Texture();

	private:
		bool valid_; //!< Is this texture valid
		int width_; //!< The width of the texture
		int height_; //!< The height of the texture
		ID3D11ShaderResourceView* texture_; //!< The actual texture
	};
}