#include "../fbx/fbx_loader.h"
#include "../application/game.h"

#include "../d3d11/d3d11_vertex_buffer.h"

#include <algorithm>

using namespace fbxsdk_2015_1;

namespace snuffbox
{
	//----------------------------------------------------------------------------------------
	bool FBXLoader::VertexSorter::operator()(Vertex& a, Vertex& b)
	{
		return a.material_id < b.material_id;
	}

	//----------------------------------------------------------------------------------------
	FBXLoader::FBXLoader() : 
		fbx_manager_(nullptr),
		fbx_scene_(nullptr)
	{
		
	}

	//----------------------------------------------------------------------------------------
	FBXLoader* FBXLoader::Instance()
	{
		static SharedPtr<FBXLoader> fbx_loader = AllocatedMemory::Instance().Construct<FBXLoader>();
		return fbx_loader.get();
	}

	//----------------------------------------------------------------------------------------
	void FBXLoader::Initialise()
	{
		fbx_manager_ = FbxManager::Create();
		SNUFF_ASSERT_NOTNULL(fbx_manager_, "FBXLoader::Initialise::fbx_manager_");
		SNUFF_LOG_INFO("FBX SDK version " + std::string(fbx_manager_->GetVersion()));

		FbxIOSettings* io_settings = FbxIOSettings::Create(fbx_manager_, IOSROOT);
		fbx_manager_->SetIOSettings(io_settings);

		fbx_scene_ = FbxScene::Create(fbx_manager_, "FBX_Scene");
		SNUFF_ASSERT_NOTNULL(fbx_scene_, "FBXLoader::Initialise::fbx_scene_");

		SNUFF_LOG_SUCCESS("Succesfully initialised the FBX loader");
	}

	//----------------------------------------------------------------------------------------
	FBXData FBXLoader::Load(const std::string& path)
	{
		SNUFF_ASSERT_NOTNULL(fbx_manager_, "FBXLoader::Load::fbx_manager_");
		bool result = true;
		std::string full_path = Game::Instance()->path() + "/" + path;
		LoadScene(full_path);
		
		SNUFF_ASSERT_NOTNULL(fbx_scene_, "FBXLoader::Load::fbx_scene_");
		FBXData data;

		FbxNode* node = fbx_scene_->GetRootNode()->GetChild(0);
		if (node)
		{
			FbxMesh* mesh = node->GetMesh();
			if (mesh)
			{
				GetMeshData(&data.vertices, &data.indices, mesh);
			}
		}

		return data;
	}

	//----------------------------------------------------------------------------------------
	void FBXLoader::LoadScene(const std::string& path)
	{
		SNUFF_ASSERT_NOTNULL(fbx_manager_, "FBXLoader::LoadScene::fbx_manager_");
		int file_major_version, file_minor_version, file_revision;
		bool result = true;

		FbxImporter* fbx_importer = FbxImporter::Create(fbx_manager_, "");

		result = fbx_importer->Initialize(path.c_str(), -1, fbx_manager_->GetIOSettings());
		SNUFF_XASSERT(result == true, "Failed importing " + path + " into the FBX manager!\n" + fbx_importer->GetStatus().GetErrorString(), "FBXLoader::LoadScene::" + path);
		fbx_importer->GetFileVersion(file_major_version, file_minor_version, file_revision);

		if (fbx_importer->IsFBX())
		{
			SNUFF_LOG_INFO("FBX file version " + std::to_string(file_major_version) + "." + std::to_string(file_minor_version) + "." + std::to_string(file_revision));
			fbx_manager_->GetIOSettings()->SetBoolProp(IMP_FBX_MATERIAL, false);
			fbx_manager_->GetIOSettings()->SetBoolProp(IMP_FBX_TEXTURE, false);
			fbx_manager_->GetIOSettings()->SetBoolProp(IMP_FBX_LINK, false);
			fbx_manager_->GetIOSettings()->SetBoolProp(IMP_FBX_SHAPE, true);
			fbx_manager_->GetIOSettings()->SetBoolProp(IMP_FBX_GOBO, false);
			fbx_manager_->GetIOSettings()->SetBoolProp(IMP_FBX_ANIMATION, true);
			fbx_manager_->GetIOSettings()->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
		}

		result = fbx_importer->Import(fbx_scene_);
		SNUFF_XASSERT(result == true, "Failed importing the file to the FBX scene", "FBXLoader::Load::" + path);

		fbx_importer->Destroy();
	}

	//----------------------------------------------------------------------------------------
	void FBXLoader::GetMeshData(std::vector<Vertex>* verts, std::vector<int>* indices, FbxMesh* mesh)
	{
		FbxStringList uv_names;
		mesh->GetUVSetNames(uv_names);
    FbxArray<int> m_indices;
    FbxLayerElementArrayTemplate<int>* material_array = nullptr;
    
    if (mesh->GetLayerCount() >= 1){
      FbxLayerElementMaterial* layer_material = mesh->GetLayer(0)->GetMaterials();
      FbxLayerElementMaterial::EReferenceMode ref_mode;

      if (layer_material != nullptr)
      {
        ref_mode = layer_material->GetReferenceMode();

        if (ref_mode == FbxLayerElementMaterial::EReferenceMode::eIndexToDirect || ref_mode == FbxLayerElementMaterial::EReferenceMode::eIndex)
        {
          material_array = &layer_material->GetIndexArray();
          SNUFF_LOG_INFO("Found " + std::to_string(material_array->GetCount()) + " material sub-groups");
        }
      }
    }

		for (unsigned int i = 0; i < static_cast<unsigned int>(uv_names.GetCount()); ++i)
		{
			const char* uv_name = uv_names.GetStringAt(i);
			const FbxGeometryElementUV* uv_element = mesh->GetElementUV(uv_name);

			if (!uv_element)
				continue;

			SNUFF_XASSERT(uv_element->GetMappingMode() == FbxGeometryElement::eByPolygonVertex || uv_element->GetMappingMode() == FbxGeometryElement::eByControlPoint, "Invalid UV mapping mode", "FBXLoader::GetMeshData");

			bool use_index = uv_element->GetReferenceMode() != FbxGeometryElement::eDirect;
			unsigned int index_count = use_index ? uv_element->GetIndexArray().GetCount() : 0;

			unsigned int poly_count = mesh->GetPolygonCount();
			FbxVector4* vertices = mesh->GetControlPoints();
			Vertex vert;
			FbxVector4 normal;
			unsigned int control_point;

			for (unsigned int polygon = 0; polygon < poly_count; ++polygon)
			{
				unsigned int poly_size = mesh->GetPolygonSize(polygon);
				SNUFF_XASSERT(poly_size == 3, "You should triangulate the mesh before using it (this is an export option in any Autodesk FBX exporter)", "FBXLoader::GetMeshData");
				
				for (unsigned int vertex = 0; vertex < poly_size; ++vertex)
				{
					control_point = mesh->GetPolygonVertex(polygon, vertex);
					mesh->GetPolygonVertexNormal(polygon, vertex, normal);
					vert.position.x = static_cast<float>(vertices[control_point].mData[0]);
					vert.position.z = static_cast<float>(-vertices[control_point].mData[1]);
					vert.position.y = static_cast<float>(-vertices[control_point].mData[2]);
					vert.position.w = 1.0f;
					vert.normal.x = static_cast<float>(-normal.mData[0]);
          vert.normal.z = static_cast<float>(normal.mData[1]);
          vert.normal.y = static_cast<float>(normal.mData[2]);
          vert.colour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

          if (material_array != nullptr)
          {
            vert.material_id = material_array->GetAt(polygon);
          }

					verts->push_back(vert);
					indices->push_back(polygon * 3 + vertex);
				}
			}

			unsigned int count = 0;
			int m_idx = -1;

			if (uv_element->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				for (unsigned int polygon = 0; polygon < poly_count; ++polygon)
				{
					unsigned int poly_size = mesh->GetPolygonSize(polygon);
					SNUFF_XASSERT(poly_size == 3, "You should triangulate the mesh before using it (this is an export option in any Autodesk FBX exporter)", "FBXLoader::GetMeshData");

					for (unsigned int vertex = 0; vertex < poly_size; ++vertex)
					{
						unsigned int control_point = mesh->GetPolygonVertex(polygon, vertex);
						unsigned int uv_index = use_index ? uv_element->GetIndexArray().GetAt(control_point) : control_point;
				
						FbxVector2 uv = uv_element->GetDirectArray().GetAt(uv_index);
						Vertex& vert = verts->at(count);
						
						vert.tex_coords.x = static_cast<float>(uv.mData[0]);
						vert.tex_coords.y = static_cast<float>(-uv.mData[1]);

						++count;
					}
				}
			}
			else if (uv_element->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				unsigned int poly_counter = 0;
				for (unsigned int polygon = 0; polygon < poly_count; ++polygon)
				{
					unsigned int poly_size = mesh->GetPolygonSize(polygon);
					SNUFF_XASSERT(poly_size == 3, "You should triangulate the mesh before using it (this is an export option in any Autodesk FBX exporter)", "FBXLoader::GetMeshData");

					for (unsigned int vertex = 0; vertex < poly_size; ++vertex)
					{
						if (poly_counter < index_count)
						{
							unsigned int uv_index = use_index ? uv_element->GetIndexArray().GetAt(poly_counter) : poly_counter;

							FbxVector2 uv = uv_element->GetDirectArray().GetAt(uv_index);

							Vertex& vert = verts->at(count);

							vert.tex_coords.x = static_cast<float>(uv.mData[0]);
							vert.tex_coords.y = static_cast<float>(-uv.mData[1]);

							++count;
							++poly_counter;
						}
					}
				}
			}
		}

		SNUFF_LOG_INFO("Vertices: " + std::to_string(verts->size()));
	}

	//----------------------------------------------------------------------------------------
	FBXLoader::~FBXLoader()
	{
		fbx_manager_->Destroy();
	}
}