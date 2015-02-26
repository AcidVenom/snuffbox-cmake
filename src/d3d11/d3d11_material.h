#pragma once

#include "../d3d11/d3d11_render_device.h"
#include "../content/content.h"

namespace snuffbox
{
	class D3D11Material : public Content
	{
	public:
		/// Default constructor
		D3D11Material();

		/// @see snuffbox::Content::Load
		void Load(const std::string& path);

		/// Applies this material
		void Apply();

		/**
		* @return snuffbox::D3D11Effect* The effect of this material
		*/
		D3D11Effect* effect();

		/**
		* @brief Sets the effect of this material
		* @param[in] effect (snuffbox::D3D11Effect*) The effect to set
		*/
		void set_effect(D3D11Effect* effect);

		/// Default destructor
		~D3D11Material();

	private:
		D3D11Texture* diffuse_;
		D3D11Texture* normal_;
		D3D11Texture* bump_;
		D3D11Effect* effect_;
	};
}