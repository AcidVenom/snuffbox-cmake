#pragma once

#include "../content/content.h"
#include "../d3d11/d3d11_vertex_buffer.h"

namespace snuffbox
{
  struct MaterialIndices;

	/**
	* @class snuffbox::FBXModel
	* @brief Holds information about an FBX model (materials, UVs, normals, vertices, indices)
	* @author Daniël Konings
	*/
	class FBXModel : public Content
	{
	public:

		/// Default constructor
		FBXModel();

		/// @see snuffbox::Content::Load
		void Load(const std::string& path);

		/**
		* @return snuffbox::D3D11VertexBuffer* The vertex buffer of this model
		*/
		D3D11VertexBuffer* vertex_buffer();

    /**
    * @return std::vector<snuffbox::MaterialIndices>& The material indices of this model
    */
    std::vector<MaterialIndices>& material_indices();

		/// Default destructor
		~FBXModel();

	private:
		SharedPtr<D3D11VertexBuffer> vertex_buffer_; //!< The vertex buffer of this model
    std::vector<MaterialIndices> material_indices_; //!< The material indices of this model
	};
}
