#include "../../d3d11/elements/d3d11_terrain_element.h"
#include "../../d3d11/d3d11_vertex_buffer.h"

#undef max

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
	D3D11Terrain::D3D11Terrain() :
		width_(256),
		height_(256)
  {
    Create(width_, height_);
  }

  //-------------------------------------------------------------------------------------------
	D3D11Terrain::D3D11Terrain(JS_ARGS args) :
		D3D11RenderElement(args),
		width_(256),
		height_(256)
  {
		Create(width_, height_);
  }

  //-------------------------------------------------------------------------------------------
	void D3D11Terrain::Create(const int& w, const int& h)
  {
		width_ = w;
		height_ = h;

		vertices_.clear();
		indices_.clear();

    vertex_buffer_ = AllocatedMemory::Instance().Construct<D3D11VertexBuffer>(D3D11VertexBuffer::VertexBufferType::kOther);
		vertex_buffer_->set_topology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (int y = 0; y < height_; y++)
		{
			for (int x = 0; x < width_; x++)
			{
				Vertex vertex;

				vertex.position = XMFLOAT4(static_cast<float>(x), 0.0f, static_cast<float>(y), 1.0f);
				vertex.normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
				vertex.tex_coords = XMFLOAT2(static_cast<float>(x) / width_, static_cast<float>(y) / height_);
				vertex.colour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				vertex.normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

				vertices_.push_back(vertex);
			}
		}
		for (int y = 0; y < height_ - 1; ++y)
		{
			for (int x = 0; x < width_ - 1; ++x)
			{
				indices_.push_back(y * width_ + x);
				indices_.push_back((y + 1) * width_ + x);
				indices_.push_back((y + 1) * width_ + x + 1);

				indices_.push_back(y * width_ + x);
				indices_.push_back((y + 1) * width_ + x + 1);
				indices_.push_back(y * width_ + x + 1);
			}
		}

		vertex_buffer_->Create(vertices_, indices_);
  }

	//-------------------------------------------------------------------------------------------
	D3D11Terrain::Indices D3D11Terrain::WorldToIndex(const float& x, const float& y, bool* in_bounds_x, bool* in_bounds_y)
	{
    *in_bounds_x = true;
    *in_bounds_y = true;

    float xx = x;
    float yy = y;

    XMVECTOR p = XMVectorSet(xx, 0.0f, yy, 1.0f);

    p = XMVector3Transform(p, XMMatrixInverse(&XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), world_matrix()));
    xx = XMVectorGetX(p);
    yy = XMVectorGetZ(p);

		if (xx > width_ - 1)
		{
      *in_bounds_x = false;
		}
		else if (xx < 0)
		{
      *in_bounds_x = false;
		}

		if (yy > height_ - 1)
		{
      *in_bounds_y = false;
		}
		else if (yy < 0)
		{
      *in_bounds_y = false;
		}

    return Indices{ static_cast<int>(xx), static_cast<int>(yy) };
	}

	//-------------------------------------------------------------------------------------------
	D3D11Terrain::WorldCoordinates D3D11Terrain::IndexToWorld(const int& x, const int& y)
	{
		XMFLOAT4 pos;
		XMStoreFloat4(&pos, translation());

		XMFLOAT4 scaling;
		XMStoreFloat4(&scaling, scale());

    XMFLOAT4 offsetting;
    XMStoreFloat4(&offsetting, offset());

		float xx = static_cast<float>(x);
		float yy = static_cast<float>(y);

    XMVECTOR p = XMVectorSet(xx, 0.0f, yy, 1.0f);

    p = XMVector3Transform(p, world_matrix());

		return WorldCoordinates{ XMVectorGetX(p), XMVectorGetZ(p) };
	}

	//-------------------------------------------------------------------------------------------
	std::vector<D3D11Terrain::Indices> D3D11Terrain::NearestVertices(const float& x, const float& y)
	{
		std::vector<Indices> indices;

    bool in_bounds_x;
    bool in_bounds_y;

    Indices v1 = WorldToIndex(x, y, &in_bounds_x, &in_bounds_y);

		Indices v2 = { v1.x + 1, v1.y };
		Indices v3 = { v1.x, v1.y + 1 };
		Indices v4 = { v1.x + 1, v1.y + 1 };

		WorldCoordinates w1 = IndexToWorld(v1.x, v1.y);
		WorldCoordinates w4 = IndexToWorld(v4.x, v4.y);

		float r = x - w1.x;

		float yy = w1.z + (w4.z - w1.z) * r;

		indices.push_back(v1);

		if (y < yy)
		{
			indices.push_back(v2);
		}
		else
		{
			indices.push_back(v3);
		}

		indices.push_back(v4);

		auto clamp = [this](Indices& index)
		{
			if (index.x < 0)
			{
				index.x = 0;
			}
			else if (index.x >= this->width_)
			{
				index.x = this->width_ - 1;
			}

			if (index.y < 0)
			{
				index.y = 0;
			}
			else if (index.y >= this->height_)
			{
				index.y = this->height_ - 1;
			}
		};

		for (unsigned int i = 0; i < indices.size(); ++i)
		{
			clamp(indices.at(i));
		}

		return indices;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Terrain::SetHeight(const int& x, const int& y, const float& h)
	{
		int idx = y * width_ + x;

		if (idx >= width_ * height_ || idx < 0)
		{
			return;
		}

		vertices_.at(idx).position.y = -h;
		SetNormals(x, y);
	}

	//-------------------------------------------------------------------------------------------
	float D3D11Terrain::GetHeight(const int& x, const int& y)
	{
		int idx = y * width_ + x;

		if (idx >= width_ * height_ || idx < 0)
		{
			return 0.0f;
		}

		return vertices_.at(idx).position.y;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Terrain::SetNormals(const int& x, const int& y)
	{
		int index_a = y * width_ + x;
		int index_b = (y + 1) * width_ + x;
		int index_c = (y + 1) * width_ + (x + 1);

		CalculateNormals(index_a, index_b, index_c);

		index_c = index_a;
		index_a = (y - 1) * width_ + x - 1;
		index_b = y * width_ + x - 1;

		CalculateNormals(index_a, index_b, index_c);

		index_b = index_c;
		index_a = (y - 1) * width_ + x;
		index_c = y * width_ + x + 1;

		CalculateNormals(index_a, index_b, index_c);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Terrain::CalculateNormals(const int& index_a, const int& index_b, const int& index_c)
	{
		int size = width_ * height_;
		if (index_a < 0 || index_b < 0 || index_c < 0 || index_a >= size || index_b >= size || index_c >= size)
		{
			return;
		}

		Vertex& vertex_a = vertices_.at(index_a);
		Vertex& vertex_b = vertices_.at(index_b);
		Vertex& vertex_c = vertices_.at(index_c);
		
		XMVECTOR p1 = XMLoadFloat4(&vertex_a.position);
		XMVECTOR p2 = XMLoadFloat4(&vertex_b.position);
		XMVECTOR p3 = XMLoadFloat4(&vertex_c.position);

		XMVECTOR cross_a = XMVectorSubtract(p2, p1);
		XMVECTOR cross_b = XMVectorSubtract(p3, p1);

		XMVECTOR normal = XMVector3Cross(cross_a, cross_b);
		vertex_a.normal = XMFLOAT3(XMVectorGetX(normal), XMVectorGetY(normal), XMVectorGetZ(normal));
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Terrain::Flush()
	{
		vertex_buffer_->Create(vertices_, indices_);
	}

  //-------------------------------------------------------------------------------------------
	D3D11VertexBuffer* D3D11Terrain::vertex_buffer()
  {
    return vertex_buffer_.get();
  }

	//-------------------------------------------------------------------------------------------
	const int& D3D11Terrain::width() const
	{
		return width_;
	}

	//-------------------------------------------------------------------------------------------
	const int& D3D11Terrain::height() const
	{
		return height_;
	}

  //-------------------------------------------------------------------------------------------
	D3D11Terrain::~D3D11Terrain()
  {

  }

  //-------------------------------------------------------------------------------------------
	void D3D11Terrain::RegisterJS(JS_CONSTRUCTABLE obj)
  {
    D3D11RenderElement::Register(obj);

		JSFunctionRegister funcs[] = {
			{ "create", JSCreate },
			{ "width", JSWidth },
			{ "height", JSHeight },
			{ "worldToIndex", JSWorldToIndex },
			{ "indexToWorld", JSIndexToWorld },
			{ "nearestVertices", JSNearestVertices },
			{ "setHeight", JSSetHeight },
			{ "getHeight", JSGetHeight },
			{ "flush", JSFlush }
		};

		JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
  }

	//-------------------------------------------------------------------------------------------
	void D3D11Terrain::JSCreate(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Terrain* self = wrapper.GetPointer<D3D11Terrain>(args.This());

		if (wrapper.Check("NN") == true)
		{
			self->Create(wrapper.GetValue<int>(0, 1), wrapper.GetValue<int>(1, 1));
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Terrain::JSWidth(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Terrain* self = wrapper.GetPointer<D3D11Terrain>(args.This());

		wrapper.ReturnValue<int>(self->width());
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Terrain::JSHeight(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Terrain* self = wrapper.GetPointer<D3D11Terrain>(args.This());

		wrapper.ReturnValue<int>(self->height());
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Terrain::JSWorldToIndex(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Terrain* self = wrapper.GetPointer<D3D11Terrain>(args.This());

		if (wrapper.Check("NN") == true)
		{
      bool in_bounds_x;
      bool in_bounds_y;

			Indices indices = self->WorldToIndex(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f), &in_bounds_x, &in_bounds_y);

			v8::Handle<v8::Object> obj = JSWrapper::CreateObject();

      if (in_bounds_x == true)
			{
				JSWrapper::SetObjectValue(obj, "x", indices.x);
			}
			
      if (in_bounds_y == true)
			{
				JSWrapper::SetObjectValue(obj, "y", indices.y);
			}

			wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Terrain::JSIndexToWorld(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Terrain* self = wrapper.GetPointer<D3D11Terrain>(args.This());

		if (wrapper.Check("NN") == true)
		{
			WorldCoordinates coords = self->IndexToWorld(wrapper.GetValue<int>(0, 0), wrapper.GetValue<int>(1, 0));

			v8::Handle<v8::Object> obj = JSWrapper::CreateObject();
			
			JSWrapper::SetObjectValue(obj, "x", coords.x);
			JSWrapper::SetObjectValue(obj, "z", coords.z);

			wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Terrain::JSNearestVertices(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Terrain* self = wrapper.GetPointer<D3D11Terrain>(args.This());

		if (wrapper.Check("NN") == true)
		{
			std::vector<Indices> verts = self->NearestVertices(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f));

			v8::Handle<v8::Object> obj = JSWrapper::CreateObject();

			for (unsigned int i = 0; i < verts.size(); ++i)
			{
				const Indices& index = verts.at(i);
				v8::Handle<v8::Object> v = JSWrapper::CreateObject();
				JSWrapper::SetObjectValue(v, "x", index.x);
				JSWrapper::SetObjectValue(v, "y", index.y);

				JSWrapper::SetObjectValue(obj, std::to_string(i), v);
			}

      wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Terrain::JSSetHeight(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Terrain* self = wrapper.GetPointer<D3D11Terrain>(args.This());

		if (wrapper.Check("NNN") == true)
		{
			self->SetHeight(wrapper.GetValue<int>(0, 0), wrapper.GetValue<int>(1, 0), wrapper.GetValue<float>(2, 0.0f));
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Terrain::JSGetHeight(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Terrain* self = wrapper.GetPointer<D3D11Terrain>(args.This());

		if (wrapper.Check("NN") == true)
		{
			wrapper.ReturnValue<float>(self->GetHeight(wrapper.GetValue<int>(0, 0), wrapper.GetValue<int>(1, 0)));
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Terrain::JSFlush(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Terrain* self = wrapper.GetPointer<D3D11Terrain>(args.This());

		self->Flush();
	}
}