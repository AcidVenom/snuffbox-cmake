#include "../../d3d11/elements/d3d11_terrain_element.h"
#include "../../d3d11/d3d11_vertex_buffer.h"

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

    vertex_buffer_ = AllocatedMemory::Instance().Construct<D3D11VertexBuffer>(D3D11VertexBuffer::VertexBufferType::kOther);
		std::vector<Vertex> vertices;
		std::vector<int> indices;

		for (int y = 0; y < height_; y++)
		{
			for (int x = 0; x < width_; x++)
			{
				Vertex vertex;

				vertex.position = XMFLOAT4(static_cast<float>(x), 0.0f, static_cast<float>(y), 1.0f);
				vertex.normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
				vertex.tex_coords = XMFLOAT2(static_cast<float>(x) / width_, 1.0f - (static_cast<float>(y) / height_));
				vertex.colour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				vertex.normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

				vertices.push_back(vertex);
			}
		}
		for (int y = 0; y < height_ - 1; ++y)
		{
			if (y % 2 == 0)
			{
				for (int x = 0; x < width_; ++x)
				{

					indices.push_back(y*width_ + x);
					indices.push_back((y + 1)* width_ + x);

					if (x == width_ - 1)
					{
						indices.push_back((y + 1)*width_ + x);
					}
				}
			}
			else
			{
				for (int x = width_ - 1; x >= 0; --x)
				{
					indices.push_back(y*width_ + x);
					indices.push_back((y + 1)* width_ + x);

					if (x == 0)
					{
						indices.push_back((y + 1)*width_ + x);
					}
				}
			}
		}

		vertex_buffer_->Create(vertices, indices);
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
			{ "height", JSHeight }
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
}