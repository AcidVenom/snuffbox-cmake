#include "../d3d11/d3d11_vertex_buffer.h"
#include "../d3d11/d3d11_shader.h"

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
  D3D11VertexBuffer::D3D11VertexBuffer(const D3D11VertexBuffer::VertexBufferType& type) :
    type_(type),
    vertex_buffer_(nullptr),
    index_buffer_(nullptr),
    valid_(false),
    topology_(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP)
  {
  
  }

  //-------------------------------------------------------------------------------------------
  void D3D11VertexBuffer::Create(const std::vector<Vertex>& verts, const std::vector<int>& indices)
  {
    if (valid_ == true)
    {
      SNUFF_SAFE_RELEASE(vertex_buffer_, "D3D11VertexBuffer::Create::vertex_buffer_");
      SNUFF_SAFE_RELEASE(index_buffer_, "D3D11VertexBuffer::Create::index_buffer_");
      valid_ = false;
    }

    vertices_.clear();
    indices_.clear();

    vertices_ = verts;
    indices_ = indices;

    SNUFF_XASSERT(vertices_.size() > 0, "There are no vertices to create a vertex buffer with", "D3D11VertexBuffer::Create");
    SNUFF_XASSERT(indices_.size() > 0, "There are no indices to create an index buffer with", "D3D11VertexBuffer::Create");

    D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();

    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices_.size());
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA input;
    ZeroMemory(&input, sizeof(D3D11_SUBRESOURCE_DATA));

    input.pSysMem = &vertices_[0];

    HRESULT result = render_device->device()->CreateBuffer(&desc, &input, &vertex_buffer_);
    SNUFF_XASSERT(result == S_OK, render_device->HRToString(result, "CreateBuffer"), "D3D11VertexBuffer::Create");

    ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = static_cast<UINT>(sizeof(int) * indices_.size());
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    ZeroMemory(&input, sizeof(D3D11_SUBRESOURCE_DATA));

    input.pSysMem = &indices_[0];

    result = render_device->device()->CreateBuffer(&desc, &input, &index_buffer_);
    SNUFF_XASSERT(result == S_OK, render_device->HRToString(result, "CreateBuffer"), "D3D11VertexBuffer::Create");

    valid_ = true;
  }

  //-------------------------------------------------------------------------------------------
  void D3D11VertexBuffer::Set()
  {
    if (valid_ == false)
    {
      SNUFF_LOG_WARNING("Attempted to set an invalid vertex buffer");
      return;
    }
    D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
    ID3D11DeviceContext* ctx = render_device->context();

    int type = static_cast<int>(type_);
		int old = render_device->vertex_buffer_type();
		if (type_ == VertexBufferType::kOther || old != type)
    {
      render_device->set_vertex_buffer_type(type);

      UINT offset = 0;
      ctx->IASetVertexBuffers(0, 1, &vertex_buffer_, &Vertex::stride_size, &offset);
      ctx->IASetIndexBuffer(index_buffer_, DXGI_FORMAT_R32_UINT, 0);
    }

    D3D11_PRIMITIVE_TOPOLOGY topology;
    ctx->IAGetPrimitiveTopology(&topology);

    if (topology != topology_)
    {
      ctx->IASetPrimitiveTopology(topology_);
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11VertexBuffer::Draw()
  {
    if (valid_ == false)
    {
      SNUFF_LOG_WARNING("Attempted to draw an invalid vertex buffer");
      return;
    }
		D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
		D3D11Shader* shader = render_device->current_shader();
		if (shader == nullptr || shader->valid() == false)
		{
			return;
		}
		
    render_device->context()->DrawIndexed(static_cast<UINT>(indices_.size()), 0, 0);
  }

  //-------------------------------------------------------------------------------------------
  const D3D11VertexBuffer::VertexBufferType& D3D11VertexBuffer::type() const
  {
    return type_;
  }

  //-------------------------------------------------------------------------------------------
  const D3D11_PRIMITIVE_TOPOLOGY& D3D11VertexBuffer::topology() const
  {
    return topology_;
  }

  //-------------------------------------------------------------------------------------------
  void D3D11VertexBuffer::set_topology(const D3D11_PRIMITIVE_TOPOLOGY& topology)
  {
    topology_ = topology;
  }

  //-------------------------------------------------------------------------------------------
  D3D11VertexBuffer::~D3D11VertexBuffer()
  {
    if (valid_ == false)
    {
      return;
    }
    SNUFF_SAFE_RELEASE(vertex_buffer_, "D3D11VertexBuffer::~D3D11VertexBuffer::vertex_buffer_");
    SNUFF_SAFE_RELEASE(index_buffer_, "D3D11VertexBuffer::~D3D11VertexBuffer::index_buffer_");
  }
}