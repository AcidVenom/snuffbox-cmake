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
		XMMATRIX InvViewProjection;
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
	* @struct snuffbox::CbLight
	* @brief A per-render-target/per-light light buffer
	* @author Daniël Konings
	*/
	__declspec(align(16)) struct CbLight
	{
		D3D11Light::Attributes Attributes;
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

		/**
		* @brief Retrieves the singleton instance of this class
		* @return snuffbox::D3D11ConstantBuffer* The pointer to the singleton instance of this class
		*/
		static D3D11ConstantBuffer* Instance();

		/// Creates the constant buffer
		void Create();
		
		/**
		* @brief Maps the constant buffer with global information
		* @param[in] cb (const snuffbox::CbGlobal&) The global constant buffer
		*/
		void Map(const CbGlobal& cb);

		/**
		* @brief Maps the constant buffer with per-object information
		* @param[in] cb (const snuffbox::CbPerObject&) The per-object buffer
		*/
		void Map(const CbPerObject& cb);

		/**
		* @brief Maps the constant buffer with uniforms information
		* @param[in] cb (const snuffbox::CbUniforms&) The uniforms buffer
		* @param[in] num_uniforms (const int&) The number of uniforms
		*/
		void Map(const CbUniforms& cb, const int& num_uniforms);

		/**
		* @brief Maps the constant buffer with light information
		* @param[in] cb (const snuffbox::CbUniforms&) The light buffer
		*/
		void Map(const CbLight& cb);

		/// Sets the constant buffer at a given index
		void Set();

		/// Default denstructor
		~D3D11ConstantBuffer();

	private:
		bool valid_; //!< Is this constant buffer valid?
		ID3D11Buffer* global_buffer_; //!< The global constant buffer
		ID3D11Buffer* per_object_buffer_; //!< The per-object constant buffer
		ID3D11Buffer* uniforms_buffer_; //!< The uniforms constant buffer
		ID3D11Buffer* light_buffer_; //!< The light constant buffer
	};
}