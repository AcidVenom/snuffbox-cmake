#pragma once

#include "../d3d11/d3d11_render_device.h"

namespace snuffbox
{
	/**
	* @struct snuffbox::CbGlobal
	* @brief A global context buffer that is mapped once before drawing
	* @author Daniël Konings
	*/
	struct CbGlobal
	{
		float Time;
		XMMATRIX View;
		XMMATRIX Projection;
	};

	/**
	* @struct snuffbox::CbPerObject
	* @brief A per-object context buffer that is mapped each time before rendering an element
	* @author Daniël Konings
	*/
	struct CbPerObject
	{
		XMMATRIX World;
		XMMATRIX InvWorld;
		float Alpha;
		XMFLOAT3 Blend;
		XMFLOAT4 AnimationCoords;
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
		* @param[in] snuffbox::CbGlobal The global constant buffer
		*/
		void Map(CbGlobal cb);

		/**
		* @brief Maps the constant buffer with per-object information
		* @param[in] snuffbox::CbPerObject The per-object buffer
		*/
		void Map(CbPerObject cb);

		/**
		* @brief Sets the constant buffer at a given index
		* @param[in] index (int) The index to map the buffer to
		*/
		void Set(int index);

		/// Default denstructor
		~D3D11ConstantBuffer();

	private:
		bool valid_; //!< Is this constant buffer valid?
		ID3D11Buffer* global_buffer_; //!< The global constant buffer
		ID3D11Buffer* per_object_buffer_; //!< The per-object constant buffer
		ID3D11Buffer* mapped_; //!< What kind of constant buffer is this?
	};
}