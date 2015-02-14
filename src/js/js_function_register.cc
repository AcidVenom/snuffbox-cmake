#pragma once

#include "../js/js_function_register.h"
#include "../js/js_state_wrapper.h"

namespace snuffbox
{
  //---------------------------------------------------------------------------------------------------------
  void JSFunctionRegister::Register(JSFunctionRegister* funcs, int num, v8::Handle<v8::ObjectTemplate> obj)
  {
    JSStateWrapper* wrapper = JSStateWrapper::Instance();
    v8::Isolate* isolate = wrapper->isolate();

    for (int i = 0; i < num; ++i)
    {
      obj->Set(isolate, funcs[i].name.c_str(), v8::FunctionTemplate::New(isolate, funcs[i].callback));
    }
  }

  //---------------------------------------------------------------------------------------------------------
  void JSFunctionRegister::Register(JSFunctionRegister* funcs, int num, v8::Handle<v8::Object> obj)
  {
    JSStateWrapper* wrapper = JSStateWrapper::Instance();
    v8::Isolate* isolate = wrapper->isolate();

    for (int i = 0; i < num; ++i)
    {
      obj->Set(v8::String::NewFromUtf8(isolate, funcs[i].name.c_str()), v8::Function::New(isolate, funcs[i].callback));
    }
  }
}