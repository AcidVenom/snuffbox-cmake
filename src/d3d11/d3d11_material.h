#pragma once

#include "../d3d11/d3d11_render_device.h"
#include "../d3d11/d3d11_texture.h"
#include "../content/content.h"

namespace snuffbox
{
	class D3D11Material : public Content
	{
	public:
		/**
		* @struct snuffbox::D3D11Material::Attributes
		* @brief Contains different attributes for a material to use
		* @author Daniël Konings
		*/
		struct Attributes
		{
			XMFLOAT4 emissive;
			XMFLOAT4 diffuse;
			XMFLOAT4 ambient;
			XMFLOAT4 specular;

			float specular_intensity;
			float reflectivity;
		};

	public:
		/// Default constructor
		D3D11Material();

		/// @see snuffbox::Content::Load
		void Load(const std::string& path);

		/// Applies this material
		void Apply();

		/**
		* @brief Retrieves a float4 value from a given JSON array
		* @param[in] arr (const v8::Local<v8::Object>&) The array to retrieve data from
		* @param[out] store (XMFLOAT4*) The buffer to store found data in
		* @return bool Was it a success?
		*/
		bool GetFloat4FromArray(const v8::Local<v8::Object>& arr, XMFLOAT4* store);

		/**
		* @return snuffbox::D3D11Effect* The effect of this material
		*/
		D3D11Effect* effect();

		/**
		* @return const snuffbox::D3D11Material::Attributes& Retrieves the attributes of this material
		*/
		const Attributes& attributes() const;

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
		SharedPtr<D3D11Texture> cube_map_;
		D3D11Effect* effect_;
		D3D11Texture::Cube cube_;
		Attributes attributes_;
	};
}