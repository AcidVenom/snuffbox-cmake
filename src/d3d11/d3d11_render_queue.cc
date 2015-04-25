#include "../d3d11/d3d11_render_queue.h"
#include "../d3d11/elements/d3d11_render_element.h"
#include "../d3d11/d3d11_vertex_buffer.h"
#include "../d3d11/d3d11_constant_buffer.h"
#include "../d3d11/d3d11_camera.h"
#include "../d3d11/d3d11_effect.h"
#include "../d3d11/d3d11_material.h"
#include "../d3d11/d3d11_render_target.h"
#include "../d3d11/d3d11_uniforms.h"
#include "../d3d11/elements/d3d11_text_element.h"
#include "../application/game.h"

#include <algorithm>

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
  bool RenderSorterZ::operator()(D3D11RenderElement* a, D3D11RenderElement* b)
  {
    return a == nullptr || b == nullptr ? false : XMVectorGetZ(a->translation()) > XMVectorGetZ(b->translation());
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
  D3D11RenderQueue::D3D11RenderQueue(D3D11RenderTarget* target) :
    target_(target)
  {

  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderQueue::Add(D3D11RenderElement* element)
  {
    std::vector<D3D11RenderElement*>& add_to =
      element->layer_type() == D3D11RenderElement::LayerType::kWorld ? world_ : ui_;

    for (D3D11RenderElement* it : add_to)
    {
      if (it == element)
      {
        return;
      }
    }
    add_to.push_back(element);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderQueue::Sort(const D3D11RenderQueue::SortMethods& method)
  {
    switch (method)
    {
    case SortMethods::kZSorting:
      std::sort(world_.begin(), world_.end(), RenderSorterZ());
      break;

    case SortMethods::kDistanceFromCamera:
      std::sort(world_.begin(), world_.end(), RenderSorterDistance());
      break;
    }

    std::sort(ui_.begin(), ui_.end(), RenderSorterZ());
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderQueue::DrawElement(ID3D11DeviceContext* context, D3D11RenderElement* element)
  {
    D3D11ConstantBuffer* constant_buffer = D3D11RenderDevice::Instance()->constant_buffer();

    D3D11Material::Attributes attributes;

    D3D11Material* material = element->material();

    Animation* animation = element->animation();

    if (animation != nullptr)
    {
      animation->Animate(static_cast<float>(Game::Instance()->delta_time()));
    }

    XMVECTOR deter;
    constant_buffer->Map({
      element->world_matrix(),
      element->billboarding() == true ? XMMatrixInverse(&deter, element->world_matrix()) : XMMatrixTranspose(XMMatrixInverse(&deter, element->world_matrix())),
      element->animation_coordinates(),
      element->blend(),
      element->alpha(),
      material == nullptr || material->is_valid() == false ? attributes : material->attributes()
    });

    element->uniforms()->Apply();

    D3D11VertexBuffer* buffer = element->vertex_buffer();
    if (buffer == nullptr)
    {
      return;
    }
    buffer->Set();

    if (material != nullptr && material->is_valid() == true)
    {
      material->Apply(element->override_diffuse(), element->override_normal(), element->override_specular(), element->override_light());
      D3D11Effect* effect = material->effect();
      D3D11Effect* override_effect = element->override_effect();
      if (override_effect != nullptr)
      {
        effect = override_effect;
      }

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
      else
      {
        element->set_material(D3D11RenderDevice::Instance()->default_material());
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

    for (int i = static_cast<int>(world_.size()) - 1; i >= 0; --i)
    {
      element = world_.at(i);

      if (element != nullptr && element->spawned() == true && element->target() == target_)
      {
        DrawElement(context, element);
      }
      else if (target_ != nullptr)
      {
        world_.erase(world_.begin() + i);
      }
    }

    D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
    render_device->MapUIBuffer();

    for (int i = static_cast<int>(ui_.size()) - 1; i >= 0; --i)
    {
      element = ui_.at(i);

      if (element->spawned() == true)
      {
        D3D11Text* text = dynamic_cast<D3D11Text*>(element);

        if (text != nullptr)
        {
          if (text->shadow_set() == true)
          {
            XMFLOAT3 blend = text->blend();
            float alpha = text->alpha();

            text->PrepareShadow();
            DrawElement(context, element);
            text->Reset(blend, alpha);
          }
        }

        DrawElement(context, element);

        if (text != nullptr)
        {
          text->DrawIcons();
        }
      }
      else if (target_ != nullptr)
      {
        ui_.erase(ui_.begin() + i);
      }
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderQueue::Clear()
  {
    D3D11RenderElement* it = nullptr;
    for (unsigned int i = 0; i < world_.size(); ++i)
    {
      it = world_.at(i);

      if (it != nullptr)
      {
        it->Destroy();
      }
    }

    for (unsigned int i = 0; i < ui_.size(); ++i)
    {
      it = ui_.at(i);

      if (it != nullptr)
      {
        it->Destroy();
      }
    }

    world_.clear();
    ui_.clear();
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderQueue::Remove(D3D11RenderElement* ptr)
  {
    D3D11RenderElement* it = nullptr;
    for (unsigned int i = 0; i < world_.size(); ++i)
    {
      it = world_.at(i);
      if (it == ptr && it != nullptr)
      {
        world_.at(i) = nullptr;
        it->Destroy();
      }
    }

    for (unsigned int i = 0; i < ui_.size(); ++i)
    {
      it = ui_.at(i);
      if (it == ptr && it != nullptr)
      {
        ui_.at(i) = nullptr;
        it->Destroy();
      }
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderQueue::set_target(D3D11RenderTarget* target)
  {
    target_ = target;
  }

  //-------------------------------------------------------------------------------------------
  D3D11RenderQueue::~D3D11RenderQueue()
  {

  }
}