#include "../d3d11/d3d11_vertex_buffer.h"
#include "../d3d11/d3d11_shader.h"
#include "../d3d11/d3d11_render_settings.h"

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
  D3D11VertexBuffer::D3D11VertexBuffer(const D3D11VertexBuffer::VertexBufferType& type) :
    type_(type),
    vertex_buffer_(nullptr),
    index_buffer_(nullptr),
    vertex_size_(0),
    index_size_(0),
		num_indices_(-1),
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

    CalculateBounds();

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

    CalculateBounds();
	}

  //-------------------------------------------------------------------------------------------
  void D3D11VertexBuffer::CalculateBounds()
  {
    float min_x = 0.0f, min_y = 0.0f, min_z = 0.0f;
    float max_x = 0.0f, max_y = 0.0f, max_z = 0.0f;

    for (unsigned int i = 0; i < vertices_.size(); ++i)
    {
      const XMFLOAT4& p = vertices_.at(i).position;

      if (p.x < min_x)
      {
        min_x = p.x;
      }
      else if (p.x > max_x)
      {
        max_x = p.x;
      }

      if (p.y < min_y)
      {
        min_y = p.y;
      }
      else if (p.y > max_y)
      {
        max_y = p.y;
      }

      if (p.z < min_z)
      {
        min_z = p.z;
      }
      else if (p.z > max_z)
      {
        max_z = p.z;
      }
    }

    float x = (max_x - min_x) / 2.0f;
    float y = (max_y - min_y) / 2.0f;
    float z = (max_z - min_z) / 2.0f;

    bounds_.Center = XMFLOAT3(min_x + x, (min_y + y) * -1, min_z + z);
    bounds_.Extents = XMFLOAT3(x, y, z);
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
      ctx->IASetVertexBuffers(0, 1, &vertex_buffer_, &Vertex::STRIDE_SIZE, &offset);
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
		
    render_device->context()->DrawIndexed(num_indices_ >= 0 ? num_indices_ : static_cast<UINT>(indices_.size()), 0, 0);
  }

  //-------------------------------------------------------------------------------------------
  bool D3D11VertexBuffer::Pick(const XMFLOAT3& origin, const XMFLOAT3& dir, float* distance, const XMMATRIX& world)
  {
		float lowest = 0.0f;
		bool found = false;

		XMVECTOR deter;
		XMMATRIX inv = XMMatrixInverse(&deter, world);

		XMVECTOR ray_dir = XMVector3Normalize(XMLoadFloat3(&dir));
		XMVECTOR ray_pos = XMVector3TransformCoord(XMLoadFloat3(&origin), inv);

		XMVECTOR v1, v2, v3;
		XMVECTOR e1, e2;

		XMVECTOR edge_dir;

		float d, inv_d;

		XMVECTOR dist_vec;

		float u, v;
		float dist;

		static const float tiny = 0.000001f;

		XMVECTOR y_down = XMVectorSet(1.0f, D3D11RenderSettings::Instance()->invert_y() == false ? -1.0f : 1.0f, 1.0f, 1.0f);

		for (int i = 0; i < indices_.size() / 3; i++)
		{
			v1 = XMLoadFloat4(&vertices_.at(indices_.at((i * 3) + 0)).position);
			v2 = XMLoadFloat4(&vertices_.at(indices_.at((i * 3) + 1)).position);
			v3 = XMLoadFloat4(&vertices_.at(indices_.at((i * 3) + 2)).position);

			v1 *= y_down;
			v2 *= y_down;
			v3 *= y_down;

			e1 = v2 - v1;
			e2 = v3 - v1;

			edge_dir = XMVector3Cross(ray_dir, e2);

			d = XMVectorGetX(XMVector3Dot(e1, edge_dir));

			if (d < -tiny)
			{
				continue;
			}

			inv_d = 1.0f / d;

			dist_vec = ray_pos - v1;

			u = XMVectorGetX(XMVector3Dot(dist_vec, edge_dir)) * inv_d;

			if (u < -tiny || u > 1.0f + tiny)
			{
				continue;
			}

			edge_dir = XMVector3Cross(dist_vec, e1);

			v = XMVectorGetX(XMVector3Dot(ray_dir, edge_dir)) * inv_d;

			if (v < -tiny || u + v > 1.0f + tiny)
			{
				continue;
			}

			dist = XMVectorGetX(XMVector3Dot(e2, edge_dir)) * inv_d;

			if (dist < 0.0f)
			{
				continue;
			}

			if (found == false)
			{
				found = true;
				lowest = dist;
			}
			else if (dist < lowest)
			{
				lowest = dist;
			}
		}

		*distance = lowest;
    return found;
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
  const BoundingBox& D3D11VertexBuffer::bounds() const
  {
    return bounds_;
  }

  //-------------------------------------------------------------------------------------------
  void D3D11VertexBuffer::set_topology(const D3D11_PRIMITIVE_TOPOLOGY& topology)
  {
    topology_ = topology;
  }

	//-------------------------------------------------------------------------------------------
	void D3D11VertexBuffer::set_num_indices(const int& n)
	{
		num_indices_ = n;
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