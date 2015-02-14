#include "../js/js_object_register.h"

#include "../application/logging.h"
#include "../cvar/cvar.h"

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
  void JSRegister::RegisterSingletons()
  {
    JSObjectRegister<DebugLogging>::RegisterSingleton();
    JSObjectRegister<CVar>::RegisterSingleton();
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