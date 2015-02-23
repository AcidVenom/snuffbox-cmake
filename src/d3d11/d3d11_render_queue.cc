#include "../d3d11/d3d11_render_queue.h"
#include "../d3d11/elements/d3d11_render_element.h"
#include "../d3d11/d3d11_vertex_buffer.h"

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
  D3D11RenderQueue::D3D11RenderQueue()
  {

  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderQueue::Add(D3D11RenderElement* element)
  {
    elements_.push_back(element);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderQueue::Sort(D3D11RenderQueue::SortMethods method)
  {

  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderQueue::DrawElement(ID3D11DeviceContext* context, D3D11RenderElement* element)
  {
    D3D11VertexBuffer* buffer = element->vertex_buffer();
    buffer->Set();
    buffer->Draw();
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderQueue::Draw(ID3D11DeviceContext* context)
  {
    for (D3D11RenderElement* element : elements_)
    {
      DrawElement(context, element);
    }
  }

  //-------------------------------------------------------------------------------------------
  D3D11RenderQueue::~D3D11RenderQueue()
  {

  }
}