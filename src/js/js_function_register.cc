#pragma once

#include "../js/js_function_register.h"

namespace snuffbox
{
  //---------------------------------------------------------------------------------------------------------
  void JSFunctionRegister::Register(JSFunctionRegister* funcs, const int& num, const v8::Handle<v8::ObjectTemplate>& obj)
  {
    JSStateWrapper* wrapper = JSStateWrapper::Instance();
    v8::Isolate* isolate = wrapper->isolate();

    for (int i = 0; i < num; ++i)
    {
			v8::Local<v8::FunctionTemplate> func = v8::FunctionTemplate::New(isolate, funcs[i].callback);
      obj->Set(isolate, funcs[i].name.c_str(), func);
    }
  }

  //---------------------------------------------------------------------------------------------------------
  void JSFunctionRegister::Register(JSFunctionRegister* funcs, const int& num, const v8::Handle<v8::Object>& obj)
  {
    JSStateWrapper* wrapper = JSStateWrapper::Instance();
    v8::Isolate* isolate = wrapper->isolate();

    for (int i = 0; i < num; ++i)
    {
			v8::Local<v8::Function> func = v8::Function::New(isolate, funcs[i].callback);
			func->SetName(v8::String::NewFromUtf8(isolate, funcs[i].name.c_str()));
      obj->Set(v8::String::NewFromUtf8(isolate, funcs[i].name.c_str()), func);
    }
  }
}