#pragma once

#include <fbxsdk.h>
#include "../memory/shared_ptr.h"
#include "../fbx/fbx_model.h"

#include <vector>

namespace snuffbox
{
	struct Vertex;

	/**
	* @struct snuffbox::FBXData
	* @brief Contains FBX data to create a vertex buffer with
	* @author Daniël Konings
	*/
	struct FBXData
	{
		std::vector<Vertex> vertices;
		std::vector<int> indices;
	};

	/**
	* @class snuffbox::FBXLoader
	* @brief A helper class to load FBX models using the Autodesk FBX SDK
	* @author Daniël Konings
	*/
	class FBXLoader
	{
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
		* @param[out] verts (std::vector<Vertex>*) The vertex vector to fill with vertex information
		* @param[out] indices (std::vector<Vertex>*) The vertex vector to fill with vertex information
		* @param[in] mesh (FbxMesh*) The mesh to load the information from
		*/
		void GetMeshData(std::vector<Vertex>* verts, std::vector<int>* indices, FbxMesh* mesh);

		/// Default destructor
		~FBXLoader();

	private:
		FbxManager*			fbx_manager_; //!< The FBX manager from the SDK
		FbxScene*				fbx_scene_; //!< The FBX scene to use
	};
}