#include "../../d3d11/elements/d3d11_terrain_element.h"
#include "../../d3d11/d3d11_material.h"
#include "../../d3d11/d3d11_render_target.h"
#include "../../d3d11/d3d11_viewport.h"
#include "../../d3d11/d3d11_shader.h"
#include "../../d3d11/d3d11_vertex_buffer.h"
#include "../../content/content_manager.h"

#undef max

static int terrain_textures = -1;

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
	D3D11Terrain::D3D11Terrain() :
		width_(256),
		height_(256),
    texture_tiling_(1.0f, 1.0f),
    brush_shader_(nullptr)
  {
    Create(width_, height_);
  }

  //-------------------------------------------------------------------------------------------
	D3D11Terrain::D3D11Terrain(JS_ARGS args) :
		D3D11RenderElement(args),
		width_(256),
		height_(256),
    texture_tiling_(1.0f, 1.0f),
    brush_shader_(nullptr)
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

    D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
    vertex_buffer_ = AllocatedMemory::Instance().Construct<D3D11VertexBuffer>(D3D11VertexBuffer::VertexBufferType::kOther);

    PrepareTextures();

    material_ = AllocatedMemory::Instance().Construct<D3D11Material>();
    
    D3D11Material* default_mat = render_device->default_material();
    material_->set_effect(default_mat->effect());
    material_->set_textures(diffuse_map_.get(), normal_map_.get(), specular_map_.get());
    material_->Validate();

		for (int y = 0; y < height_; y++)
		{
			for (int x = 0; x < width_; x++)
			{
				Vertex vertex;

				vertex.position = XMFLOAT4(static_cast<float>(x), 0.0f, static_cast<float>(y), 1.0f);
				vertex.normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
        vertex.tex_coords = XMFLOAT2(static_cast<float>(x) / width_ * texture_tiling_.x, static_cast<float>(y) / height_ * texture_tiling_.y);
				vertex.colour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				vertex.normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
				vertex.tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);
				vertex.bitangent = XMFLOAT3(0.0f, 0.0f, 1.0f);

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

		vertex_buffer_->Create(vertices_, indices_, false);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Terrain::PrepareTextures()
  {
    D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();

    ++terrain_textures;
    diffuses_ = AllocatedMemory::Instance().Construct<D3D11RenderTarget>("TerrainTextureDiffuse" + std::to_string(terrain_textures));
    diffuses_->Create(D3D11RenderTarget::RenderTargets::kRenderTarget, render_device->swap_chain(), render_device->device(), width_, height_);

    normals_ = AllocatedMemory::Instance().Construct<D3D11RenderTarget>("TerrainTextureNormals" + std::to_string(terrain_textures));
    normals_->Create(D3D11RenderTarget::RenderTargets::kRenderTarget, render_device->swap_chain(), render_device->device(), width_, height_);

    speculars_ = AllocatedMemory::Instance().Construct<D3D11RenderTarget>("TerrainTextureSpeculars" + std::to_string(terrain_textures));
    speculars_->Create(D3D11RenderTarget::RenderTargets::kRenderTarget, render_device->swap_chain(), render_device->device(), width_, height_);

    diffuses_->AddMultiTarget(normals_.get());
    diffuses_->AddMultiTarget(speculars_.get());

    render_device->AddRenderTarget(diffuses_.get());

    brush_vp_ = AllocatedMemory::Instance().Construct<D3D11Viewport>();
    brush_vp_->Create(0.0f, 0.0f, static_cast<float>(width_), static_cast<float>(height_));

    diffuse_map_ = AllocatedMemory::Instance().Construct<D3D11Texture>();
    diffuse_map_->Create(diffuses_->resource());

    normal_map_ = AllocatedMemory::Instance().Construct<D3D11Texture>();
    normal_map_->Create(normals_->resource());

    specular_map_ = AllocatedMemory::Instance().Construct<D3D11Texture>();
    specular_map_->Create(speculars_->resource());
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

		if (xx > width_)
		{
      *in_bounds_x = false;
		}
		else if (xx < 0)
		{
      *in_bounds_x = false;
		}

		if (yy > height_)
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

		return -vertices_.at(idx).position.y;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Terrain::SetNormals(const int& x, const int& y)
	{
    auto clamp_get = [this, x, y](int xx, int yy)
    {
      xx = x + xx;
      yy = y + yy;

      if (xx < 0)
      {
        xx = 0;
      }

      if (xx >= width_)
      {
        xx = width_ - 1;
      }

      if (yy < 0)
      {
        yy = 0;
      }

      if (yy >= height_)
      {
        yy = height_ - 1;
      }

      return yy * width_ + xx;
    };

		for (int xx = -1; xx <= 1; ++xx)
		{
			for (int yy = -1; yy <= 1; ++yy)
			{
				CalculateNormals(clamp_get(xx, yy), clamp_get(xx + 1, yy + 1), clamp_get(xx, yy + 1));
			}
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Terrain::CalculateNormals(const int& a, const int& b, const int& c)
	{
		Vertex& va = vertices_.at(a);
		Vertex& vb = vertices_.at(b);
		Vertex& vc = vertices_.at(c);

		XMVECTOR e1 = XMLoadFloat4(&vc.position) - XMLoadFloat4(&va.position);
		XMVECTOR e2 = XMLoadFloat4(&vb.position) - XMLoadFloat4(&va.position);

		XMFLOAT3 normal;
		XMStoreFloat3(&normal, XMVector3Normalize(XMVector3Cross(e1, e2)));

		normal.z *= -1;
		normal.x *= -1;

		va.normal = normal;
	}

  //-------------------------------------------------------------------------------------------
  XMFLOAT2 D3D11Terrain::GetWorldTextureCoordinates(const float& x, const float& y)
  {
    XMVECTOR p1 = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    XMVECTOR p2 = XMVectorSet(static_cast<float>(width_), 0.0f, static_cast<float>(height_), 1.0f);
    XMVECTOR deter = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

    XMVECTOR p = XMVectorSet(x, 0.0f, y, 1.0f);
    p = XMVector4Transform(p, XMMatrixInverse(&deter, world_matrix()));

    XMVECTOR offset = p - p1;
    XMVECTOR tex_coords = offset / p2;

    XMFLOAT2 to_return;
    XMStoreFloat2(&to_return, tex_coords);

    to_return.x *= texture_tiling_.x;
    to_return.y *= texture_tiling_.y;

    return to_return;
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Terrain::BrushTexture(const std::string& brush, const std::string& texture, const float& x, const float& y, const float& radius)
  {
    if (brush_shader_ == nullptr)
    {
      SNUFF_LOG_ERROR("The shader 'brush.fx' was never loaded, thus there cannot be brushed to the terrain");
      return;
    }

    ContentManager* content_manager = ContentManager::Instance();

    D3D11Texture* brush_texture = content_manager->Get<D3D11Texture>(brush);
    D3D11Texture* diffuse = content_manager->Get<D3D11Texture>(texture);

    if (brush_texture == nullptr || diffuse == nullptr)
    {
      SNUFF_LOG_ERROR("Tried to paint onto the terrain with an invalid brush or diffuse texture");
      return;
    }

    D3D11VertexBuffer brush_vbo(D3D11VertexBuffer::VertexBufferType::kOther);

    std::vector<int> indices = {
      0, 1, 3, 0, 3, 2
    };

    float hr = radius / 2.0f;
    XMFLOAT2 t1 = GetWorldTextureCoordinates(x - hr, y - hr);
    XMFLOAT2 t2 = GetWorldTextureCoordinates(x + hr, y - hr);
    XMFLOAT2 t3 = GetWorldTextureCoordinates(x - hr, y + hr);
    XMFLOAT2 t4 = GetWorldTextureCoordinates(x + hr, y + hr);

    XMVECTOR deter = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

    XMVECTOR pv1 = XMVectorSet(x - hr, 0.0f, y - hr, 1.0f);
    XMVECTOR pv2 = XMVectorSet(x + hr, 0.0f, y - hr, 1.0f);
    XMVECTOR pv3 = XMVectorSet(x - hr, 0.0f, y + hr, 1.0f);
    XMVECTOR pv4 = XMVectorSet(x + hr, 0.0f, y + hr, 1.0f);

    XMVECTOR div = XMVectorSet(static_cast<float>(width_), 0.0f, static_cast<float>(height_), 1.0f);
    XMVECTOR subtract = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);

    pv1 = (XMVector4Transform(pv1, XMMatrixInverse(&deter, world_matrix())) / div) * 2.0f - subtract;
    pv2 = (XMVector4Transform(pv2, XMMatrixInverse(&deter, world_matrix())) / div) * 2.0f - subtract;
    pv3 = (XMVector4Transform(pv3, XMMatrixInverse(&deter, world_matrix())) / div) * 2.0f - subtract;
    pv4 = (XMVector4Transform(pv4, XMMatrixInverse(&deter, world_matrix())) / div) * 2.0f - subtract;

    XMFLOAT4 p1, p2, p3, p4;

    XMStoreFloat4(&p1, pv1);
    XMStoreFloat4(&p2, pv2);
    XMStoreFloat4(&p3, pv3);
    XMStoreFloat4(&p4, pv4);

    p1.y = p1.z;
    p2.y = p2.z;
    p3.y = p3.z;
    p4.y = p4.z;

    p1.z = p2.z = p3.z = p4.z = 0.0f;

    brush_vbo.Create({
      Vertex(p1, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), t1, XMFLOAT3(0.0f, 0.0f, 1.0f)),
      Vertex(p2, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), t2, XMFLOAT3(0.0f, 0.0f, 1.0f)),
      Vertex(p3, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), t3, XMFLOAT3(0.0f, 0.0f, 1.0f)),
      Vertex(p4, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), t4, XMFLOAT3(0.0f, 0.0f, 1.0f))
    },
    indices);
    
    D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();

    diffuses_->Set(render_device->context());
    brush_vbo.Set();
    brush_vp_->Set();
    brush_shader_->Set();

    D3D11Texture::SetMultipleTextures(0, 2, { brush_texture, diffuse });
    brush_vbo.Draw();
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Terrain::Flush()
  {
    vertex_buffer_->Create(vertices_, indices_, false);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Terrain::set_texture_tiling(const float& u, const float& v)
  {
    texture_tiling_.x = u;
    texture_tiling_.y = v;

    Flush();
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