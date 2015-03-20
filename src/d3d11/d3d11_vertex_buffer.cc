#include "../d3d11/d3d11_vertex_buffer.h"
#include "../d3d11/d3d11_shader.h"

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
  D3D11VertexBuffer::D3D11VertexBuffer(const D3D11VertexBuffer::VertexBufferType& type) :
    type_(type),
    vertex_buffer_(nullptr),
    index_buffer_(nullptr),
		vertex_size_(0),
		index_size_(0),
    valid_(false),
    topology_(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
  {
  
  }

  //-------------------------------------------------------------------------------------------
	void D3D11VertexBuffer::Create(const std::vector<Vertex>& verts, const std::vector<int>& indices, const bool& tangents)
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

		if (tangents == true)
		{
			CalculateTangents();
		}

    SNUFF_XASSERT(vertices_.size() > 0, "There are no vertices to create a vertex buffer with", "D3D11VertexBuffer::Create");
    SNUFF_XASSERT(indices_.size() > 0, "There are no indices to create an index buffer with", "D3D11VertexBuffer::Create");

    D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();

    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

		desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices_.size());
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA input;
    ZeroMemory(&input, sizeof(D3D11_SUBRESOURCE_DATA));

    input.pSysMem = &vertices_[0];

    HRESULT result = render_device->device()->CreateBuffer(&desc, &input, &vertex_buffer_);
    SNUFF_XASSERT(result == S_OK, render_device->HRToString(result, "CreateBuffer"), "D3D11VertexBuffer::Create");

    ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = static_cast<UINT>(sizeof(int) * indices_.size());
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;

    ZeroMemory(&input, sizeof(D3D11_SUBRESOURCE_DATA));

    input.pSysMem = &indices_[0];

    result = render_device->device()->CreateBuffer(&desc, &input, &index_buffer_);
    SNUFF_XASSERT(result == S_OK, render_device->HRToString(result, "CreateBuffer"), "D3D11VertexBuffer::Create");

		vertex_size_ = static_cast<unsigned int>(vertices_.size());
		index_size_ = static_cast<unsigned int>(indices_.size());

    valid_ = true;
  }

	//-------------------------------------------------------------------------------------------
	void D3D11VertexBuffer::Update(const std::vector<Vertex>& verts, const std::vector<int>& indices, const bool& tangents)
	{
		if (tangents == true)
		{
			CalculateTangents();
		}
		
		if (valid_ == false)
		{
			SNUFF_LOG_WARNING("Attempted to update an invalid vertex buffer");
			return;
		}

		if (vertex_size_ != verts.size() || index_size_ != indices.size())
		{
			SNUFF_LOG_ERROR("Could not update vertex buffer, the current vertices/indices sizes are not equal to the ones buffered");
			return;
		}

		vertices_ = verts;
		indices_ = indices;

		Vertex* v = nullptr;

		D3D11_MAPPED_SUBRESOURCE data;
		ID3D11DeviceContext* ctx = D3D11RenderDevice::Instance()->context();

		ctx->Map(vertex_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
		v = static_cast<Vertex*>(data.pData);
		for (unsigned int i = 0; i < vertex_size_; ++i)
		{
			v[i] = vertices_.at(i);
		}
		ctx->Unmap(vertex_buffer_, 0);
		
		int* idx = nullptr;

		ctx->Map(index_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
		idx = static_cast<int*>(data.pData);
		for (unsigned int i = 0; i < index_size_; ++i)
		{
			idx[i] = indices_.at(i);
		}
		ctx->Unmap(index_buffer_, 0);
	}

  //-------------------------------------------------------------------------------------------
  void D3D11VertexBuffer::CalculateTangents()
  {
		unsigned int idx1, idx2, idx3;
		XMVECTOR e1, e2, t1, t2;
		XMVECTOR p, uv, tangent, bitangent;
		XMFLOAT3 store;

    if (static_cast<float>(indices_.size()) / 3.0f != std::floor(static_cast<float>(indices_.size()) / 3.0f))
    {
      SNUFF_LOG_ERROR("Could not calculate tangents for a given vertex buffer, this is according to the indices size\n\
        Triangle list topology can be used to prevent this (Given the right indices are used)");
      return;
    }

		auto average_tangents = [this](Vertex& v1, const unsigned int& skip)
		{
			XMVECTOR old;
			XMVECTOR tangent = XMLoadFloat3(&v1.tangent);
			XMVECTOR bitangent = XMLoadFloat3(&v1.bitangent);

			for (unsigned int i = 0; i < indices_.size(); ++i)
			{
				if (i == skip)
				{
					continue;
				}
				Vertex& v2 = vertices_.at(indices_.at(i));

				if (v1.position.x == v2.position.x && v1.position.y == v2.position.y && v1.position.z == v2.position.z)
				{
					old = XMLoadFloat3(&v2.tangent);
					old += tangent;
					XMStoreFloat3(&v2.tangent, old);

					old = XMLoadFloat3(&v2.bitangent);
					old += bitangent;
					XMStoreFloat3(&v2.bitangent, old);
				}
			}
		};

		for (unsigned int i = 0; i < indices_.size(); i += 3)
		{
			idx1 = i;
			idx2 = i + 1;
			idx3 = i + 2;

			Vertex& v1 = vertices_.at(indices_.at(idx1));
			Vertex& v2 = vertices_.at(indices_.at(idx2));
			Vertex& v3 = vertices_.at(indices_.at(idx3));

			p = XMLoadFloat4(&v1.position);
			e1 = XMLoadFloat4(&v2.position) - p;
			e2 = XMLoadFloat4(&v3.position) - p;

			uv = XMLoadFloat2(&v1.tex_coords);
			t1 = XMLoadFloat2(&v2.tex_coords) - uv;
			t2 = XMLoadFloat2(&v3.tex_coords) - uv;

			float r = 1.0f / XMVectorGetX(XMVector2Cross(t1, t2));
			tangent = (e1 * XMVectorGetY(t2) - e2 * XMVectorGetY(t1)) * r;
			bitangent = (e2 * XMVectorGetX(t1) - e1 * XMVectorGetX(t2)) * r;

			XMStoreFloat3(&store, tangent);
			v1.tangent = v2.tangent = v3.tangent = store;

			XMStoreFloat3(&store, bitangent);
			v1.bitangent = v2.bitangent = v3.bitangent = store;
		}

		for (unsigned int i = 0; i < indices_.size(); ++i)
		{
			Vertex& v = vertices_.at(indices_.at(i));
			average_tangents(v, i);
		}
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
    bool set = false;

    if (type_ == VertexBufferType::kOther)
    {
      if (render_device->current_model() != this)
      {
        set = true;
      }
    }
    else if (old != type)
    {
      set = true;
    }

		if (set == true)
    {
      render_device->set_vertex_buffer_type(type);
      render_device->set_current_model(this);

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