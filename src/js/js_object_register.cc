#include "../js/js_object_register.h"

#include "../application/game.h"
#include "../application/logging.h"

#include "../cvar/cvar.h"

#include "../input/keyboard.h"
#include "../input/mouse.h"

#include "../content/content_manager.h"

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
  }

  //-------------------------------------------------------------------------------------------
  void JSRegister::RegisterConstructables()
  {
    
  }

  //-------------------------------------------------------------------------------------------
  void JSRegister::Register()
  {
    RegisterSingletons();
    RegisterConstructables();
  }
}