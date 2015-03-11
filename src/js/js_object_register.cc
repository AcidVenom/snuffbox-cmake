#include "../js/js_object_register.h"

#include "../application/game.h"
#include "../application/logging.h"

#include "../cvar/cvar.h"

#include "../input/keyboard.h"
#include "../input/mouse.h"
#include "../input/mouse_area.h"

#include "../content/content_manager.h"

#include "../io/io_manager.h"

#include "../d3d11/d3d11_render_target.h"
#include "../d3d11/d3d11_render_settings.h"
#include "../d3d11/d3d11_camera.h"
#include "../d3d11/d3d11_light.h"
#include "../d3d11/d3d11_lighting.h"
#include "../d3d11/d3d11_line.h"
#include "../d3d11/d3d11_uniforms.h"

#include "../d3d11/elements/d3d11_quad_element.h"
#include "../d3d11/elements/d3d11_terrain_element.h"
#include "../d3d11/elements/d3d11_model_element.h"
#include "../d3d11/elements/d3d11_widget_element.h"
#include "../d3d11/elements/d3d11_billboard_element.h"
#include "../d3d11/elements/d3d11_text_element.h"
#include "../d3d11/elements/d3d11_polygon_element.h"

#include "../js/js_object_register.h"

#include "../platform/platform_window.h"

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
  void JSRegister::RegisterSingletons()
  {
		JSObjectRegister<Game>::RegisterSingleton();
    JSObjectRegister<DebugLogging>::RegisterSingleton();
		JSObjectRegister<CVar>::RegisterSingleton();
    JSObjectRegister<Keyboard>::RegisterSingleton();
		JSObjectRegister<Mouse>::RegisterSingleton();
		JSObjectRegister<ContentManager>::RegisterSingleton();
		JSObjectRegister<IOManager>::RegisterSingleton();
    JSObjectRegister<D3D11RenderSettings>::RegisterSingleton();
		JSObjectRegister<D3D11Lighting>::RegisterSingleton();
		JSObjectRegister<D3D11Line>::RegisterSingleton();
		JSObjectRegister<D3D11Uniforms>::RegisterSingleton();
    JSObjectRegister<Window>::RegisterSingleton();
  }

  //-------------------------------------------------------------------------------------------
  void JSRegister::RegisterConstructables()
  {
    JSObjectRegister<MouseArea>::Register();

		JSObjectRegister<D3D11RenderTarget>::Register();
		JSObjectRegister<D3D11Camera>::Register();
		JSObjectRegister<D3D11Light>::Register();

    JSObjectRegister<D3D11Quad>::Register();
		JSObjectRegister<D3D11Terrain>::Register();
		JSObjectRegister<D3D11Model>::Register();
		JSObjectRegister<D3D11Widget>::Register();
		JSObjectRegister<D3D11Billboard>::Register();
    JSObjectRegister<D3D11Text>::Register();
    JSObjectRegister<D3D11Polygon>::Register();

		JSObjectRegister<SpriteAnimation>::Register();
  }

  //-------------------------------------------------------------------------------------------
  void JSRegister::Register()
  {
    RegisterSingletons();
    RegisterConstructables();
  }
}