#pragma once

#include "../d3d11/d3d11_render_device.h"
#include "../d3d11/d3d11_material.h"
#include "../d3d11/d3d11_light.h"

namespace snuffbox
{
	/**
	* @struct snuffbox::CbGlobal
	* @brief A global constant buffer that is mapped once before drawing
	* @author Daniël Konings
	*/
	__declspec(align(16)) struct CbGlobal
	{
		float Time;
		XMMATRIX View;
		XMMATRIX Projection;
		XMVECTOR EyePosition;
	};

	/**
	* @struct snuffbox::CbLighting
	* @brief A lighting buffer to emulate lights
	* @author Daniël Konings
	*/
	__declspec(align(16)) struct CbLighting
	{
		XMFLOAT4 Ambient;
		int NumLights;
		D3D11Light::Attributes Lights[64];
	};

	/**
	* @struct snuffbox::CbPerObject
	* @brief A per-object constant buffer that is mapped each time before rendering an element
	* @author Daniël Konings
	*/
	__declspec(align(16)) struct CbPerObject
	{
		XMMATRIX World;
		XMMATRIX InvWorld;
		XMFLOAT4 AnimationCoords;
		XMFLOAT3 Blend;
		float Alpha;

		D3D11Material::Attributes Attributes;
	};

	/**
	* @struct snuffbox::CbUniforms
	* @brief A per-object/per-render-target uniforms buffer
	* @author Daniël Konings
	*/
	__declspec(align(16)) struct CbUniforms
	{
		XMFLOAT4 Uniforms[1024];
	};

	/**
	* @class snuffbox::D3D11ConstantBuffer
	* @brief A wrapper around D3D11 constant buffers, used for per-shader variables
	* @author Daniël Konings
	*/
	class D3D11ConstantBuffer
	{
	public:
		/// Default constructor
		D3D11ConstantBuffer();

		/// Creates the constant buffer
		void Create();
		
		/**
		* @brief Maps the constant buffer with global information
		* @param[in] const snuffbox::CbGlobal& The global constant buffer
		*/
		void Map(const CbGlobal& cb);

		/**
		* @brief Maps the constant buffer with lighting information
		* @param[in] const snuffbox::CbLighting& The lighting buffer
		*/
		void Map(const CbLighting& cb, const int& num_lights);

		/**
		* @brief Maps the constant buffer with per-object information
		* @param[in] const snuffbox::CbPerObject& The per-object buffer
		*/
		void Map(const CbPerObject& cb);

		/**
		* @brief Maps the constant buffer with uniforms information
		* @param[in] const snuffbox::CbUniforms& The uniforms buffer
		*/
		void Map(const CbUniforms& cb, const int& num_uniforms);

		/**
		* @brief Sets the constant buffer at a given index
		* @param[in] index (const int&) The index to map the buffer to
		*/
		void Set(const int& index);

		/// Default denstructor
		~D3D11ConstantBuffer();

	private:
		bool valid_; //!< Is this constant buffer valid?
		ID3D11Buffer* global_buffer_; //!< The global constant buffer
		ID3D11Buffer* per_object_buffer_; //!< The per-object constant buffer
		ID3D11Buffer* lighting_buffer_; //!< The per-object constant buffer
		ID3D11Buffer* uniforms_buffer_; //!< The uniforms constant buffer
		ID3D11Buffer* mapped_; //!< What kind of constant buffer is this?
	};
}