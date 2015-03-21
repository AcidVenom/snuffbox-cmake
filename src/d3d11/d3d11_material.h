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
			XMFLOAT4 diffuse;
			XMFLOAT4 ambient;

			float specular_power;
			float specular_intensity;
			float reflectivity;
			float normal_scale;
			float emissive;
			XMFLOAT3 padding;
		};

	public:
		/// Default constructor
		D3D11Material();

		/// @see snuffbox::Content::Load
		void Load(const std::string& path);

		/**
		* @brief Applies this material
		* @param[in] override_diffuse (snuffbox::D3D11Texture*) The diffuse override, default = nullptr
		* @param[in] override_normal (snuffbox::D3D11Texture*) The normal override, default = nullptr
		* @param[in] override_specular (snuffbox::D3D11Texture*) The specular override, default = nullptr
		* @param[in] override_light (snuffbox::D3D11Texture*) The light override, default = nullptr
		*/
		void Apply(D3D11Texture* override_diffuse = nullptr, D3D11Texture* override_normal = nullptr, D3D11Texture* override_specular = nullptr, D3D11Texture* override_light = nullptr);

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
		Attributes& attributes();

		/**
		* @brief Sets the effect of this material
		* @param[in] effect (snuffbox::D3D11Effect*) The effect to set
		*/
		void set_effect(D3D11Effect* effect);

		/**
		* @brief Sets the textures this material uses
		* @param[in] diffuse (snuffbox::D3D11Texture*) The diffuse map texture
		* @param[in] normal (snuffbox::D3D11Texture*) The normal map texture
		* @param[in] specular (snuffbox::D3D11Texture*) The specular map texture
		* @param[in] light (snuffbox::D3D11Texture*) The light map texture
		*/
    void set_textures(D3D11Texture* diffuse, D3D11Texture* normal = nullptr, D3D11Texture* specular = nullptr, D3D11Texture* light = nullptr);

		/// Default destructor
		~D3D11Material();

	private:
		D3D11Texture* diffuse_;
		D3D11Texture* normal_;
		D3D11Texture* specular_;
		D3D11Texture* light_map_;
		SharedPtr<D3D11Texture> cube_map_;
		D3D11Effect* effect_;
		D3D11Texture::Cube cube_;
		Attributes attributes_;
	};
}