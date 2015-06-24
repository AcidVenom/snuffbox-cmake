#pragma once

#include <fbxsdk.h>
#include "../memory/shared_ptr.h"
#include "../fbx/fbx_model.h"

#include <vector>

namespace snuffbox
{
	struct Vertex;

  /**
  * @struct snuffbox::MaterialIndices
  * @brief Holds information about material indices, where they start and where they end
  * @author Daniël Konings
  */
  struct MaterialIndices
  {
    unsigned int start; //!< The start index of this material group
    unsigned int end; //!< The end index of this material group
    int material_id; //!< The material ID corresponding to these indices
  };

	/**
	* @struct snuffbox::FBXData
	* @brief Contains FBX data to create a vertex buffer with
	* @author Daniël Konings
	*/
	struct FBXData
	{
		std::vector<Vertex> vertices; //!< All vertices of the mesh
		std::vector<int> indices; //!< All indices of the mesh
    std::vector<MaterialIndices> materials; //!< The materials of the mesh
	};

	/**
	* @class snuffbox::FBXLoader
	* @brief A helper class to load FBX models using the Autodesk FBX SDK
	* @author Daniël Konings
	*/
	class FBXLoader
	{
	public:
		/**
		* @class snuffbox::FBXLoader::VertexSorter
		* @brief Sorts vertices by their ascending material IDs
		* @author Daniël Konings
		*/
		struct VertexSorter
		{
			bool operator()(Vertex& a, Vertex& b);
		};

	public:
		/// Default constructor
		FBXLoader();

		/**
		* @brief Retrieves the singleton instance of this object
		* @return snuffbox::FBXLoader* The pointer to the singleton instance
		*/
		static FBXLoader* Instance();

		/// Initialises the FBX loader
		void Initialise();

		/**
		* @brief Loads a model from a given path, returning the list of vertices
		* @param[in] path (const std::string&) The path to load from
		* @return (snuffbox::FBXData) The loaded data
		*/
		FBXData Load(const std::string& path);

		/**
		* @brief Loads an FBX scene from a file path
		* @param[in] path (const std::string&) The path to load from
		*/
		void LoadScene(const std::string& path);

		/**
		* @brief Retrieves all mesh data necessary
		* @param[out] data (snuffbox::FBXData*) The data structure to load the information in
		* @param[in] mesh (FbxMesh*) The mesh to load the information from
		*/
		void GetMeshData(FBXData* data, FbxMesh* mesh);

		/// Default destructor
		~FBXLoader();

	private:
		FbxManager*			fbx_manager_; //!< The FBX manager from the SDK
		FbxScene*				fbx_scene_; //!< The FBX scene to use
	};
}