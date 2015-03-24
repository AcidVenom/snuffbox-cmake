#pragma once

#include "../d3d11/d3d11_render_device.h"

namespace snuffbox
{
	/**
	* @class snuffbox::D3D11RasterizerState
	* @brief A rasterizer state for culling, fill mode and scissor 
	* @author Daniël Konings
	*/
	class D3D11RasterizerState
	{
	public:

		/// Default constructor
		D3D11RasterizerState();

		/**
		* @brief Creates the rasterizer state with a given description
		* @param[in] desc (const D3D11_RASTERIZER_DESC&) The description to create the rasterizer with
		*/
		void Create(const D3D11_RASTERIZER_DESC& desc);

		/**
		* @brief Creates the rasterizer state from a given JSON
		* @param[in] src (const std::string&) The stringified JSON input
		*/
		void CreateFromJson(const std::string& src);

		/**
		* @brief Creates the rasterizer state from a given JSON
		* @param[in] obj (const v8::Handle<v8::Object>&) The JSON object
		*/
		void CreateFromJson(const v8::Handle<v8::Object>& obj);

		/// Sets this rasterizer state for use
		void Set();

		/**
		* @brief Compares this rasterizer state to another one to see if a state change is required
		* @param[in] other (snuffbox::D3D11RasterizerState*) The other rasterizer state to compare against
		* @return bool Are they equal or not?
		*/
		bool EqualsTo(D3D11RasterizerState* other);

		/**
		* @return const D3D11_RASTERIZER_DESC& The description of this rasterizer state
		*/
		const D3D11_RASTERIZER_DESC& description() const;

		/// Default destructor
		~D3D11RasterizerState();

		/**
		* @brief Converts a string to a cull mode
		* @param[in] str (const std::string&) The string to convert
		* @return D3D11_CULL_MODE The converted value
		*/
		static D3D11_CULL_MODE StringToCullMode(const std::string& str);

		/**
		* @brief Converts a string to a fill mode
		* @param[in] str (const std::string&) The string to convert
		* @return D3D11_FILL_MODE The converted value
		*/
		static D3D11_FILL_MODE StringToFillMode(const std::string& str);

	private:
		ID3D11RasterizerState* rasterizer_; //!< The actual rasterizer state
		bool valid_; //!< Is this rasterizer state valid?
		D3D11_RASTERIZER_DESC desc_; //!< The description of this rasterizer state
	};
}