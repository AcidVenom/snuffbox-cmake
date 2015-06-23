#pragma once

#include "../js/js_wrapper.h"

namespace snuffbox
{
  /**
  * @class snuffbox::JSRegister
  * @brief This class will register all Snuffbox classes that need to be exposed to JavaScript
  * @author Daniël Konings
  */
  class JSRegister
  {
  private:
    /// Registers all singletons
    static void RegisterSingletons();

    /// Registers all constructables
    static void RegisterConstructables();

  public:
    /// Registers all snuffbox objects
    static void Register();
  };

  /**
  * @class snuffbox::JSObjectRegister<T, bool>
  * @brief Used to register a JavaScript exposed object to the JavaScript state
  * @author Daniël Konings
  */
  template<typename T>
  class JSObjectRegister
  {
  public:

    /// Registers the T object
    static void RegisterSingleton();

    /// Registers the T object as a function template
    static void Register();

		/// Uses to set the to string function of each Snuffbox exposed class
		template <typename Y>
		static void ToString(JS_ARGS args);
  };

  //---------------------------------------------------------------------------------------------------------
  template<typename T>
  inline void JSObjectRegister<T>::RegisterSingleton()
  {
    JSStateWrapper* wrapper = JSStateWrapper::Instance();
    v8::Isolate* isolate = wrapper->isolate();

    v8::HandleScope scope(isolate);

    v8::Handle<v8::Object> object = v8::Object::New(isolate);
    T::RegisterJS(object);

		object->Set(v8::String::NewFromUtf8(isolate, "toString"), v8::Function::New(isolate, JSObjectRegister::ToString<T>));
    wrapper->RegisterGlobal(T::js_name(), object);
  }

  //---------------------------------------------------------------------------------------------------------
  template<typename T>
  inline void JSObjectRegister<T>::Register()
  {
    JSStateWrapper* wrapper = JSStateWrapper::Instance();
    v8::Isolate* isolate = wrapper->isolate();

    v8::HandleScope scope(isolate);

    v8::Handle<v8::FunctionTemplate> object = v8::FunctionTemplate::New(isolate);
    T::RegisterJS(object->PrototypeTemplate());

		object->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "toString"), v8::Function::New(isolate, JSObjectRegister::ToString<T>));
    object->SetCallHandler(JSStateWrapper::JSNew<T>);
    object->SetClassName(v8::String::NewFromUtf8(isolate, T::js_name()));

    wrapper->RegisterGlobal(T::js_name(), object->GetFunction());
  }

	//---------------------------------------------------------------------------------------------------------
	template <typename T> template <typename Y>
	inline void JSObjectRegister<T>::ToString(JS_ARGS args)
	{
		JSWrapper wrapper(args);

		wrapper.ReturnValue<std::string>(Y::js_name());
	}
}