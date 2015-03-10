#include "../../d3d11/elements/d3d11_widget_element.h"
#include "../../d3d11/d3d11_vertex_buffer.h"

#include "../../input/mouse_area.h"

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

    set_technique("UI");
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Widget::AddMouseArea(MouseArea* area)
  {
    mouse_areas_.push_back(area);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Widget::RemoveMouseArea(MouseArea* area)
  {
    MouseArea* it = nullptr;
    for (unsigned int i = 0; i < mouse_areas_.size(); ++i)
    {
      it = mouse_areas_.at(i);
      if (it == area)
      {
        mouse_areas_.erase(mouse_areas_.begin() + i);
        break;
      }
    }
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
    for (unsigned int i = 0; i < mouse_areas_.size(); ++i)
    {
      mouse_areas_.at(i)->set_parent(nullptr);
    }
  }

  //-------------------------------------------------------------------------------------------
	void D3D11Widget::RegisterJS(JS_CONSTRUCTABLE obj)
  {
    D3D11RenderElement::Register(obj);
  }
}