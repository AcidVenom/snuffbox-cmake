#include "../../d3d11/elements/d3d11_widget_element.h"
#include "../../d3d11/d3d11_vertex_buffer.h"

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
	D3D11Widget::D3D11Widget()
  {
    Create();
  }

  //-------------------------------------------------------------------------------------------
	D3D11Widget::D3D11Widget(JS_ARGS args) :
		D3D11RenderElement(args)
  {
    Create();
  }

  //-------------------------------------------------------------------------------------------
	void D3D11Widget::Create()
  {
		set_layer_type(LayerType::kUI);

    vertex_buffer_ = AllocatedMemory::Instance().Construct<D3D11VertexBuffer>(D3D11VertexBuffer::VertexBufferType::kQuad);

		std::vector<int> indices = {
			0, 1, 2, 3
		};

    vertex_buffer_->Create({
      { XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
      { XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
      { XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
      { XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) }
    },
		indices);
  }

  //-------------------------------------------------------------------------------------------
	D3D11VertexBuffer* D3D11Widget::vertex_buffer()
  {
    return vertex_buffer_.get();
  }

	//-------------------------------------------------------------------------------------------
	const XMMATRIX& D3D11Widget::world_matrix()
	{
		CalculateWorldMatrix(&world_matrix_, true);

		return world_matrix_;
	}

  //-------------------------------------------------------------------------------------------
	D3D11Widget::~D3D11Widget()
  {

  }

  //-------------------------------------------------------------------------------------------
	void D3D11Widget::RegisterJS(JS_CONSTRUCTABLE obj)
  {
    D3D11RenderElement::Register(obj);
  }
}