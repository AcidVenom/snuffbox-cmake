#include "../../d3d11/elements/d3d11_render_element.h"
#include "../../d3d11/d3d11_vertex_buffer.h"
#include "../../d3d11/d3d11_render_target.h"
#include "../../d3d11/d3d11_render_queue.h"

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
  D3D11RenderElement::D3D11RenderElement()
  {

  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::Spawn(const std::string& target)
  {
    D3D11RenderDevice::Instance()->GetTarget(target)->queue()->Add(this);
  }

  //-------------------------------------------------------------------------------------------
  D3D11RenderElement::~D3D11RenderElement()
  {

  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::Register(JS_CONSTRUCTABLE obj)
  {
    JSFunctionRegister funcs[] = {
      { "spawn", JSSpawn }
    };

    JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSSpawn(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

    self->Spawn(wrapper.GetValue<std::string>(0, "undefined"));
  }
}