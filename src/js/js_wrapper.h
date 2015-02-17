#pragma once

#include <v8.h>
#include <string>

#include "../js/js_function_register.h"
#include "../js/js_state_wrapper.h"

#define JS_ARGS const v8::FunctionCallbackInfo<v8::Value>&
#define JS_SETUP(type) JSWrapper wrapper(args); type* self = wrapper.GetPointer<type>(args.This());

namespace snuffbox
{
	/**
	* @class snuffbox::JSWrapper
	* @brief A wrapper around v8::FunctionCallbackInfo, for easier data retrieval
	* @author Daniël Konings
	*/
	class JSWrapper
	{
	public:
		enum Types
		{
			kVoid,
			kNumber,
			kBoolean,
			kString,
			kObject,
			kArray,
			kFunction,
			kNull,
			kUndefined,
			kUnknown
		};
	public:
		/**
		* @brief Construct with a given argument scope
		* @param[in] args (const v8::FunctionCallbackInfo<v8::Value>&) The arguments
		*/
		JSWrapper(JS_ARGS args);

		/// Default destructor
		~JSWrapper();

    /**
    * @brief Gets a value off of the argument stack
    * @param[in] arg (int) The argument to retrieve the value of
    * @param[in] def (T) The default value if no value was found
    */
		template<typename T>
		T GetValue(int arg, T def);

    /**
    * @brief Gets a C++ pointer from a value with an external __ptr field
    * @param[in] val (v8::Handle<v8::Value>) The value to retrieve the pointer from
    * @return T* The returned pointer, nullptr if none was found
    */
    template<typename T>
    T* GetPointer(v8::Handle<v8::Value> val);

    /**
    * @brief Gets a C++ pointer from a value with an external __ptr field
    * @param[in] arg (int) The argument to retrieve the pointer from
    * @return T* The returned pointer, nullptr if none was found
    */
    template<typename T>
    T* GetPointer(int arg);

    /**
    * @brief Casts a value to a JavaScript handle
    * @param[in] val (T) The value to cast
    * @return v8::Handle<v8::Value> The casted value
    */
    template<typename T>
    static v8::Handle<v8::Value> CastValue(T val);

    /**
    * @brief Returns a value to JavaScript
    * @param[in] val (T) The value to return
    */
    template<typename T>
    void ReturnValue(T val);

    /**
    * @brief Creates a JavaScript object handle and returns it
    * @return v8::Handle<v8::Object> The created object handle
    */
    static v8::Handle<v8::Object> CreateObject();

    /**
    * @brief Returns a value to JavaScript
    * @param[in] obj (v8::Handle<v8::Object>) The object to assign the value to
    * @param[in] field (std::string) The field to set
    * @param[in] val (T) The value to set
    */
    template<typename T>
    static void SetObjectValue(v8::Handle<v8::Object> obj, std::string field, T val);

		/**
		* @brief Returns the type of a local value
		* @param[in] value (v8::Local<v8::Value>) The value to check the type of
		* @return snuffbox::JSWrapper::Types The type of the value
		*/
		static Types TypeOf(v8::Local<v8::Value> value);

		/**
		* @brief Converts a JavaScript type to a string value
		* @param[in] type (snuffbox::JSWrapper::Types) The type to convert
		* @return std::string The converted string value
		*/
		static std::string TypeToString(Types type);

		/**
		* @brief Constructs an argument error and logs it
		* @param[in] expected (snuffbox::JSWrapper::Types) The expected type
		* @param[in] got (snuffbox::JSWrapper::Types) The type that was actually received
		*/
		void Error(Types expected, Types got);

		/**
		* @brief Checks the argument scope if the format is as it should be
		* @param[in] format (std::string) The format to check
		* @return bool Was the format check completed succesfully?
		*/ 
		bool Check(std::string format);

		/**
		* @brief Disables error checking in snuffbox::JSWrapper::Check
		* @param[in] value (bool) On/off value for error checking
		*/
		void set_error_checks(bool value);

	private:
		JS_ARGS args_; //!< The cached argument scope
		bool error_checks_; //!< A boolean for multiple type checking
	};

	//-------------------------------------------------------------------------------------------
	template<>
	inline bool JSWrapper::GetValue<bool>(int arg, bool def)
	{
		if (args_[arg]->IsNull() || args_[arg]->IsUndefined())
		{
			return def;
		}

		if (args_[arg]->IsBoolean() == true)
		{
			return args_[arg]->ToBoolean()->Value();
		}
		return def;
	}

	//-------------------------------------------------------------------------------------------
	template<>
	inline int JSWrapper::GetValue<int>(int arg, int def)
	{
		if (args_[arg]->IsNull() || args_[arg]->IsUndefined())
		{
			return def;
		}

		if (args_[arg]->IsNumber() == true)
		{
			return static_cast<int>(args_[arg]->ToNumber()->Value());
		}
		return def;
	}

	//-------------------------------------------------------------------------------------------
	template<>
	inline float JSWrapper::GetValue<float>(int arg, float def)
	{
		if (args_[arg]->IsNull() || args_[arg]->IsUndefined())
		{
			return def;
		}

		if (args_[arg]->IsNumber() == true)
		{
			return static_cast<float>(args_[arg]->ToNumber()->Value());
		}
		return def;
	}

	//-------------------------------------------------------------------------------------------
	template<>
	inline double JSWrapper::GetValue<double>(int arg, double def)
	{
		if (args_[arg]->IsNull() || args_[arg]->IsUndefined())
		{
			return def;
		}

		if (args_[arg]->IsNumber() == true)
		{
			return static_cast<double>(args_[arg]->ToNumber()->Value());
		}
		return def;
	}

	//-------------------------------------------------------------------------------------------
	template<>
	inline const char* JSWrapper::GetValue<const char*>(int arg, const char* def)
	{
		if (args_[arg]->IsNull() || args_[arg]->IsUndefined())
		{
			return def;
		}

    return *v8::String::Utf8Value(args_[arg]->ToString());
	}

	//-------------------------------------------------------------------------------------------
	template<>
	inline std::string JSWrapper::GetValue<std::string>(int arg, std::string def)
	{
		if (args_[arg]->IsNull() || args_[arg]->IsUndefined())
		{
			return def;
		}

		return *v8::String::Utf8Value(args_[arg]->ToString());
	}

  //-------------------------------------------------------------------------------------------
  template<>
  inline v8::Handle<v8::Value> JSWrapper::GetValue<v8::Handle<v8::Value>>(int arg, v8::Handle<v8::Value> def)
  {
    if (args_[arg]->IsNull() || args_[arg]->IsUndefined())
    {
      return def;
    }

    if (args_[arg]->IsNull() == false)
    {
      return args_[arg];
    }
    return def;
  }

  //-------------------------------------------------------------------------------------------
  template<typename T>
  inline T* JSWrapper::GetPointer(v8::Handle<v8::Value> val)
  {
    v8::Local<v8::Object> obj = val->ToObject();
    v8::Local<v8::Value> ext = obj->GetHiddenValue(v8::String::NewFromUtf8(JSStateWrapper::Instance()->isolate(), "__ptr"));

    if (ext->IsExternal() == false)
    {
      return nullptr;
    }
    else
    {
      return static_cast<T*>(ext.As<v8::External>()->Value());
    }
  }

  //-------------------------------------------------------------------------------------------
  template<typename T>
  inline T* JSWrapper::GetPointer(int arg)
  {
    v8::Local<v8::Object> obj = args_[arg]->ToObject();
    v8::Local<v8::Value> ext = obj->GetHiddenValue(v8::String::NewFromUtf8(JSStateWrapper::Instance()->isolate(), "__ptr"));

    if (ext->IsExternal() == false)
    {
      return nullptr;
    }
    else
    {
      return static_cast<T*>(ext.As<v8::External>()->Value());
    }
  }

  //-------------------------------------------------------------------------------------------
  template<>
  inline v8::Handle<v8::Value> JSWrapper::CastValue<double>(double val)
  {
    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    return v8::Number::New(isolate, val);
  }

  //-------------------------------------------------------------------------------------------
  template<>
  inline v8::Handle<v8::Value> JSWrapper::CastValue<int>(int val)
  {
    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    return v8::Number::New(isolate, val);
  }

  //-------------------------------------------------------------------------------------------
  template<>
  inline v8::Handle<v8::Value> JSWrapper::CastValue<float>(float val)
  {
    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    return v8::Number::New(isolate, val);
  }

  //-------------------------------------------------------------------------------------------
  template<>
  inline v8::Handle<v8::Value> JSWrapper::CastValue<bool>(bool val)
  {
    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    return v8::Boolean::New(isolate, val);
  }

  //-------------------------------------------------------------------------------------------
  template<>
  inline v8::Handle<v8::Value> JSWrapper::CastValue<std::string>(std::string val)
  {
    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    return v8::String::NewFromUtf8(isolate, val.c_str());
  }

  //-------------------------------------------------------------------------------------------
  template<>
  inline v8::Handle<v8::Value> JSWrapper::CastValue<const char*>(const char* val)
  {
    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    return v8::String::NewFromUtf8(isolate, val);
  }

  //-------------------------------------------------------------------------------------------
  template<typename T>
  inline void JSWrapper::ReturnValue(T val)
  {
    args_.GetReturnValue().Set<v8::Value>(JSWrapper::CastValue<T>(val));
  }

  //-------------------------------------------------------------------------------------------
  template<>
  inline void JSWrapper::ReturnValue<v8::Handle<v8::Object>>(v8::Handle<v8::Object> val)
  {
    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    args_.GetReturnValue().Set<v8::Object>(val);
  }

  //-------------------------------------------------------------------------------------------
  template<>
  inline void JSWrapper::SetObjectValue<double>(v8::Handle<v8::Object> obj, std::string field, double val)
  {
    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    obj->Set(v8::String::NewFromUtf8(isolate, field.c_str()), v8::Number::New(isolate, val));
  }

  //-------------------------------------------------------------------------------------------
  template<>
  inline void JSWrapper::SetObjectValue<bool>(v8::Handle<v8::Object> obj, std::string field, bool val)
  {
    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    obj->Set(v8::String::NewFromUtf8(isolate, field.c_str()), v8::Boolean::New(isolate, val));
  }

  //-------------------------------------------------------------------------------------------
  template<>
  inline void JSWrapper::SetObjectValue<std::string>(v8::Handle<v8::Object> obj, std::string field, std::string val)
  {
    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    obj->Set(v8::String::NewFromUtf8(isolate, field.c_str()), v8::String::NewFromUtf8(isolate, val.c_str()));
  }

  //-------------------------------------------------------------------------------------------
  template<>
  inline void JSWrapper::SetObjectValue<const char*>(v8::Handle<v8::Object> obj, std::string field, const char* val)
  {
    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    obj->Set(v8::String::NewFromUtf8(isolate, field.c_str()), v8::String::NewFromUtf8(isolate, val));
  }

  //-------------------------------------------------------------------------------------------
  template<>
  inline void JSWrapper::SetObjectValue<v8::Handle<v8::Value>>(v8::Handle<v8::Object> obj, std::string field, v8::Handle<v8::Value> val)
  {
    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    obj->Set(v8::String::NewFromUtf8(isolate, field.c_str()), val);
  }
}