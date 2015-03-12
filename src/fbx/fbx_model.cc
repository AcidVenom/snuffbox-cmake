#include "../fbx/fbx_model.h"
#include "../fbx/fbx_loader.h"

#include "../application/game.h"

#include "../content/content_manager.h"

namespace snuffbox
{
	//----------------------------------------------------------------------------------------
	FBXModel::FBXModel() :
		Content(ContentTypes::kModel)
	{
		
	}

	//----------------------------------------------------------------------------------------
	void FBXModel::Load(const std::string& path)
	{
		vertex_buffer_ = AllocatedMemory::Instance().Construct<D3D11VertexBuffer>(D3D11VertexBuffer::VertexBufferType::kOther);

		FBXData data = FBXLoader::Instance()->Load(path);
		vertex_buffer_->Create(
			data.vertices,
			data.indices
			);
	}

	//----------------------------------------------------------------------------------------
	D3D11VertexBuffer* FBXModel::vertex_buffer()
	{
		return vertex_buffer_.get();
	}

	//----------------------------------------------------------------------------------------
	FBXModel::~FBXModel()
	{
		
	}
}