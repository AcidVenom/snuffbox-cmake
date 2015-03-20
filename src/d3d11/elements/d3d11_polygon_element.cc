#include "../../d3d11/elements/d3d11_polygon_element.h"

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
  D3D11Polygon::D3D11Polygon() : 
    topology_(static_cast<int>(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST))
  {
		vertex_buffer_ = AllocatedMemory::Instance().Construct<D3D11VertexBuffer>(D3D11VertexBuffer::VertexBufferType::kOther);
  }

  //-------------------------------------------------------------------------------------------
  D3D11Polygon::D3D11Polygon(JS_ARGS args) :
		D3D11RenderElement(args),
    topology_(static_cast<int>(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST))
  {
		vertex_buffer_ = AllocatedMemory::Instance().Construct<D3D11VertexBuffer>(D3D11VertexBuffer::VertexBufferType::kOther);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::AddVertex(const XMFLOAT4& position, const XMFLOAT4& colour, const XMFLOAT2& texcoord, const XMFLOAT3& normal, const XMFLOAT3& tangent, const XMFLOAT3& bitangent)
  {
    Vertex v(position, colour, texcoord, normal);
    v.tangent = tangent;
    v.bitangent = bitangent;

    vertices_.push_back(v);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::RemoveVertex(const int& idx)
  {
    if (idx >= vertices_.size() || idx < 0)
    {
      SNUFF_LOG_ERROR("Attempted to remove a vertex at index '" + std::to_string(idx) + "', this is out of bounds; vertices size is '" + std::to_string(vertices_.size()));

      return;
    }
    vertices_.erase(vertices_.begin() + idx);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::SetVertex(const int& idx, const XMFLOAT4& position, const XMFLOAT4& colour, const XMFLOAT2& texcoord, const XMFLOAT3& normal, const XMFLOAT3& tangent, const XMFLOAT3& bitangent)
  {
    if (idx >= vertices_.size() || idx < 0)
    {
      SNUFF_LOG_ERROR("Attempted to set a vertex at index '" + std::to_string(idx) + "', this is out of bounds; vertices size is '" + std::to_string(vertices_.size()) + "'");

      return;
    }

    Vertex& v = vertices_.at(idx);
    v.position = position;
    v.colour = colour;
    v.tex_coords = texcoord;
    v.normal = normal;
    v.tangent = tangent;
    v.bitangent = bitangent;
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::ClearVertices()
  {
    vertices_.clear();
  }

  //-------------------------------------------------------------------------------------------
  int D3D11Polygon::NumVertices()
  {
    return static_cast<int>(vertices_.size());
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::AddIndex(const int& index)
  {
    indices_.push_back(index);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::RemoveIndex(const int& idx)
  {
    if (idx >= indices_.size() || idx < 0)
    {
			SNUFF_LOG_ERROR("Attempted to remove an index at index '" + std::to_string(idx) + "', this is out of bounds; indices size is '" + std::to_string(indices_.size()) + "'");

      return;
    }
    
    indices_.erase(indices_.begin() + idx);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::SetIndex(const int& idx, const int& index)
  {
    if (idx >= indices_.size() || idx < 0)
    {
      SNUFF_LOG_ERROR("Attempted to set an index at index '" + std::to_string(idx) + "', this is out of bounds; indices size is '" + std::to_string(indices_.size()));

      return;
    }

    indices_.at(idx) = index;
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::ClearIndices()
  {
    vertices_.clear();
  }

  //-------------------------------------------------------------------------------------------
  int D3D11Polygon::NumIndices()
  {
    return static_cast<int>(indices_.size());
  }

	//-------------------------------------------------------------------------------------------
	void D3D11Polygon::Create(const bool& tangents)
	{
		if (indices_.size() == 0 || vertices_.size() == 0)
		{
			SNUFF_LOG_WARNING("Attempted to create a polygon with either 0 vertices or 0 indices, a polygon needs both vertices and indices");
			return;
		}

		vertex_buffer_->Create(vertices_, indices_, tangents);
	}

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::Flush(const bool& tangents)
  {
    if (indices_.size() == 0 || vertices_.size() == 0)
    {
      SNUFF_LOG_WARNING("Attempted to flush a polygon with either 0 vertices or 0 indices, a polygon needs both vertices and indices");
      return;
    }

    vertex_buffer_->Update(vertices_, indices_, tangents);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::set_topology(const int& topology)
  {
    topology_ = topology;
  }

  //-------------------------------------------------------------------------------------------
  D3D11VertexBuffer* D3D11Polygon::vertex_buffer()
  {
    if (vertex_buffer_ != nullptr)
    {
      vertex_buffer_->set_topology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(topology_));
      return vertex_buffer_.get();
    }
    return nullptr;
  }

  //-------------------------------------------------------------------------------------------
  D3D11Polygon::~D3D11Polygon()
  {

  }

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::RegisterJS(JS_CONSTRUCTABLE obj)
  {
    D3D11RenderElement::Register(obj);

    v8::Handle<v8::Object> enumerator = JSWrapper::CreateObject();

    JSWrapper::SetObjectValue(enumerator, "TriangleStrip", static_cast<int>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP));
    JSWrapper::SetObjectValue(enumerator, "TriangleList", static_cast<int>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
    JSWrapper::SetObjectValue(enumerator, "LineList", static_cast<int>(D3D11_PRIMITIVE_TOPOLOGY_LINELIST));
    JSWrapper::SetObjectValue(enumerator, "LineStrip", static_cast<int>(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP));

    JSStateWrapper::Instance()->RegisterGlobal("Topology", enumerator);

    JSFunctionRegister funcs[] = {
      { "addVertex", JSAddVertex },
      { "setVertex", JSSetVertex },
      { "removeVertex", JSRemoveVertex },
      { "clearVertices", JSClearVertices },
      { "numVertices", JSNumVertices },
      { "addIndex", JSAddIndex },
      { "setIndex", JSSetIndex },
      { "removeIndex", JSRemoveIndex },
      { "clearIndices", JSClearIndices },
      { "numIndices", JSNumIndices },
			{ "create", JSCreate },
      { "flush", JSFlush },
      { "setTopology", JSSetTopology },
      { "setBillboarding", JSSetBillboarding }
    };

    JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::JSAddVertex(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11Polygon* self = wrapper.GetPointer<D3D11Polygon>(args.This());

    if (wrapper.Check("NNNNNNNNNNNN") == true)
    {
      float x = wrapper.GetValue<float>(0, 0.0f);
      float y = wrapper.GetValue<float>(1, 0.0f);
      float z = wrapper.GetValue<float>(2, 0.0f);

      float r = wrapper.GetValue<float>(3, 0.0f);
      float g = wrapper.GetValue<float>(4, 0.0f);
      float b = wrapper.GetValue<float>(5, 0.0f);
      float a = wrapper.GetValue<float>(6, 0.0f);

      float tx = wrapper.GetValue<float>(7, 0.0f);
      float ty = wrapper.GetValue<float>(8, 0.0f);

      float nx = wrapper.GetValue<float>(9, 0.0f);
      float ny = wrapper.GetValue<float>(10, 0.0f);
      float nz = wrapper.GetValue<float>(11, 0.0f);

      float tanx = wrapper.GetValue<float>(12, 1.0f);
      float tany = wrapper.GetValue<float>(13, 0.0f);
      float tanz = wrapper.GetValue<float>(14, 0.0f);

      float btanx = wrapper.GetValue<float>(15, 0.0f);
      float btany = wrapper.GetValue<float>(16, 1.0f);
      float btanz = wrapper.GetValue<float>(17, 0.0f);

      self->AddVertex(XMFLOAT4(x, y, z, 1.0f), XMFLOAT4(r, g, b, a), XMFLOAT2(tx, ty), XMFLOAT3(nx, ny, nz), XMFLOAT3(tanx, tany, tanz), XMFLOAT3(btanx, btany, btanz));
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::JSSetVertex(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11Polygon* self = wrapper.GetPointer<D3D11Polygon>(args.This());

    if (wrapper.Check("NNNNNNNNNNNNN") == true)
    {
      int index = wrapper.GetValue<int>(0, 0);

      float x = wrapper.GetValue<float>(1, 0.0f);
      float y = wrapper.GetValue<float>(2, 0.0f);
      float z = wrapper.GetValue<float>(3, 0.0f);

      float r = wrapper.GetValue<float>(4, 0.0f);
      float g = wrapper.GetValue<float>(5, 0.0f);
      float b = wrapper.GetValue<float>(6, 0.0f);
      float a = wrapper.GetValue<float>(7, 0.0f);

      float tx = wrapper.GetValue<float>(8, 0.0f);
      float ty = wrapper.GetValue<float>(9, 0.0f);

      float nx = wrapper.GetValue<float>(10, 0.0f);
      float ny = wrapper.GetValue<float>(11, 0.0f);
      float nz = wrapper.GetValue<float>(12, 0.0f);

      float tanx = wrapper.GetValue<float>(13, 1.0f);
      float tany = wrapper.GetValue<float>(14, 0.0f);
      float tanz = wrapper.GetValue<float>(15, 0.0f);

      float btanx = wrapper.GetValue<float>(16, 0.0f);
      float btany = wrapper.GetValue<float>(17, 1.0f);
      float btanz = wrapper.GetValue<float>(18, 0.0f);

      self->SetVertex(index, XMFLOAT4(x, y, z, 1.0f), XMFLOAT4(r, g, b, a), XMFLOAT2(tx, ty), XMFLOAT3(nx, ny, nz), XMFLOAT3(tanx, tany, tanz), XMFLOAT3(btanx, btany, btanz));
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::JSRemoveVertex(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11Polygon* self = wrapper.GetPointer<D3D11Polygon>(args.This());

    if (wrapper.Check("N") == true)
    {
      int index = wrapper.GetValue<int>(0, 0);
      self->RemoveVertex(index);
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::JSClearVertices(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11Polygon* self = wrapper.GetPointer<D3D11Polygon>(args.This());

    self->ClearVertices();
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::JSNumVertices(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11Polygon* self = wrapper.GetPointer<D3D11Polygon>(args.This());

    wrapper.ReturnValue<int>(self->NumVertices());
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::JSAddIndex(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11Polygon* self = wrapper.GetPointer<D3D11Polygon>(args.This());

    if (wrapper.Check("N") == true)
    {
      int index = wrapper.GetValue<int>(0, 0);
      self->AddIndex(index);
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::JSSetIndex(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11Polygon* self = wrapper.GetPointer<D3D11Polygon>(args.This());

    if (wrapper.Check("NN") == true)
    {
      int idx = wrapper.GetValue<int>(0, 0);
      int index = wrapper.GetValue<int>(1, 0);

      self->SetIndex(idx, index);
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::JSRemoveIndex(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11Polygon* self = wrapper.GetPointer<D3D11Polygon>(args.This());

    if (wrapper.Check("N") == true)
    {
      int index = wrapper.GetValue<int>(0, 0);
      self->RemoveIndex(index);
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::JSClearIndices(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11Polygon* self = wrapper.GetPointer<D3D11Polygon>(args.This());

    self->ClearIndices();
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::JSNumIndices(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11Polygon* self = wrapper.GetPointer<D3D11Polygon>(args.This());

    wrapper.ReturnValue<int>(self->NumIndices());
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::JSFlush(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11Polygon* self = wrapper.GetPointer<D3D11Polygon>(args.This());
     
    self->Flush(wrapper.GetValue<bool>(0, false));
  }

	//-------------------------------------------------------------------------------------------
	void D3D11Polygon::JSCreate(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Polygon* self = wrapper.GetPointer<D3D11Polygon>(args.This());

		self->Create(wrapper.GetValue<bool>(0, false));
	}

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::JSSetTopology(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11Polygon* self = wrapper.GetPointer<D3D11Polygon>(args.This());

    if (wrapper.Check("N") == true)
    {
      self->set_topology(wrapper.GetValue<int>(0, 0));
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Polygon::JSSetBillboarding(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11Polygon* self = wrapper.GetPointer<D3D11Polygon>(args.This());

    if (wrapper.Check("B") == true)
    {
      self->set_billboarding(wrapper.GetValue<bool>(0, false));
    }
  }
}