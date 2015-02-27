#include "../d3d11/d3d11_render_queue.h"
#include "../d3d11/elements/d3d11_render_element.h"
#include "../d3d11/d3d11_vertex_buffer.h"
#include "../d3d11/d3d11_constant_buffer.h"
#include "../d3d11/d3d11_camera.h"
#include "../d3d11/d3d11_effect.h"
#include "../d3d11/d3d11_material.h"

#include <algorithm>

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	bool RenderSorterZ::operator()(D3D11RenderElement* a, D3D11RenderElement* b)
	{
		return XMVectorGetZ(a->translation()) < XMVectorGetZ(b->translation());
	}

	//-------------------------------------------------------------------------------------------
	bool RenderSorterDistance::operator()(D3D11RenderElement* a, D3D11RenderElement* b)
	{
		D3D11Camera* camera = D3D11RenderDevice::Instance()->camera();
		XMVECTOR v1 = a->translation() - camera->translation();
		XMVECTOR v2 = b->translation() - camera->translation();
		
		float x1 = XMVectorGetX(v1);
		float y1 = XMVectorGetY(v1);
		float z1 = XMVectorGetZ(v1);
		float d1 = std::sqrt(x1 * x1 + y1 * y1 + z1 * z1);

		float x2 = XMVectorGetX(v2);
		float y2 = XMVectorGetY(v2);
		float z2 = XMVectorGetZ(v2);
		float d2 = std::sqrt(x2 * x2 + y2 * y2 + z2 * z2);

		return d1 < d2;
	}

  //-------------------------------------------------------------------------------------------
  D3D11RenderQueue::D3D11RenderQueue()
  {

  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderQueue::Add(D3D11RenderElement* element)
  {
		for (D3D11RenderElement* it : elements_)
		{
			if (it == element)
			{
				return;
			}
		}
    elements_.push_back(element);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderQueue::Sort(const D3D11RenderQueue::SortMethods& method)
  {
		switch (method)
		{
		case SortMethods::kZSorting:
			std::sort(elements_.begin(), elements_.end(), RenderSorterZ());
			break;

		case SortMethods::kDistanceFromCamera:
			std::sort(elements_.begin(), elements_.end(), RenderSorterDistance());
			break;
		}
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderQueue::DrawElement(ID3D11DeviceContext* context, D3D11RenderElement* element)
  {
		D3D11ConstantBuffer* constant_buffer = D3D11RenderDevice::Instance()->per_object_buffer();

		D3D11Material::Attributes attributes;

		D3D11Material* material = element->material();

		XMVECTOR deter;
		constant_buffer->Map({
			element->world_matrix(),
			XMMatrixInverse(&deter, element->world_matrix()),
			element->alpha(),
			element->blend(),
			XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
			material == nullptr || material->is_valid() == false ? attributes : material->attributes()
		});
		constant_buffer->Set(1);

    D3D11VertexBuffer* buffer = element->vertex_buffer();
    buffer->Set();

		if (material != nullptr && material->is_valid() == true)
		{
			material->Apply();
			D3D11Effect* effect = material->effect();

			if (effect != nullptr && effect->is_valid() == true)
			{
				unsigned int n_pass = effect->NumPasses(element->technique());

				if (n_pass > 0)
				{
					for (unsigned int i = 0; i < n_pass; ++i)
					{
						effect->Apply(element->technique(), i);

						buffer->Draw();
					}
				}
				else
				{
					buffer->Draw();
				}
			}
			else
			{
				if (effect != nullptr)
				{
					material->set_effect(nullptr);
				}
			}
		}
		else
		{
			if (material != nullptr)
			{
				element->set_material(nullptr);
			}
			buffer->Draw();
		}
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderQueue::Draw(ID3D11DeviceContext* context)
  {
		Sort(D3D11RenderDevice::Instance()->camera()->type() == D3D11Camera::CameraTypes::kPerspective ?
			SortMethods::kDistanceFromCamera : SortMethods::kZSorting);
    D3D11RenderElement* element = nullptr;
    for (int i = static_cast<int>(elements_.size()) - 1; i >= 0; --i)
    {
      element = elements_.at(i);

      if (element->spawned() == true)
      {
        DrawElement(context, element);
      }
      else
      {
        elements_.erase(elements_.begin() + i);
      }
    }
  }

	//-------------------------------------------------------------------------------------------
	void D3D11RenderQueue::Clear()
	{
		for (unsigned int i = 0; i < elements_.size(); ++i)
		{
			elements_.at(i)->Destroy();
		}
	}

  //-------------------------------------------------------------------------------------------
  D3D11RenderQueue::~D3D11RenderQueue()
  {

  }
}