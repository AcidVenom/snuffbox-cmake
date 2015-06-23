#pragma once

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
    * @param[in] arg (const int&) The argument to retrieve the value of
    * @param[in] def (const T&) The default value if no value was found
    */
		template<typename T>
		T GetValue(const int& arg, const T& def);

    /**
    * @brief Gets a C++ pointer from a value with an external __ptr field
    * @param[in] val (const v8::Handle<v8::Value>&) The value to retrieve the pointer from
    * @return T* The returned pointer, nullptr if none was found
    */
    template<typename T>
    T* GetPointer(const v8::Handle<v8::Value>& val);

    /**
    * @brief Gets a C++ pointer from a value with an external __ptr field
    * @param[in] arg (const int&) The argument to retrieve the pointer from
    * @return T* The returned pointer, nullptr if none was found
    */
    template<typename T>
    T* GetPointer(const int& arg);

    /**
    * @brief Casts a value to a JavaScript handle
    * @param[in] val (const T&) The value to cast
    * @return v8::Handle<v8::Value> The casted value
    */
    template<typename T>
    static v8::Handle<v8::Value> CastValue(const T& val);

    /**
    * @brief Returns a value to JavaScript
    * @param[in] val (const T&) The value to return
    */
    template<typename T>
    void ReturnValue(const T& val);

    /**
    * @brief Creates a JavaScript object handle and returns it
    * @return v8::Handle<v8::Object> The created object handle
    */
    static v8::Handle<v8::Object> CreateObject();

		/**
		* @brief Creates a JavaScript array handle and returns it
		* @return v8::Handle<v8::Array> The created array handle
		*/
		static v8::Handle<v8::Array> CreateArray();

    /**
    * @brief Sets an object value
    * @param[in] obj (const v8::Handle<v8::Object>&) The object to assign the value to
    * @param[in] field (const std::string&) The field to set
    * @param[in] val (const T&) The value to set
    */
    template<typename T>
    static void SetObjectValue(const v8::Handle<v8::Object>& obj, const std::string& field, const T& val);

		/**
		* @brief Sets an array value
		* @param[in] obj (const v8::Handle<v8::Array>&) The array to assign the value to
		* @param[in] idx (const int&) The index to set
		* @param[in] val (const T&) The value to set
		*/
		template<typename T>
		static void SetArrayValue(const v8::Handle<v8::Array>& obj, const int& idx, const T& val);

		/**
		* @brief Returns the type of a local value
		* @param[in] value (const v8::Local<v8::Value>&) The value to check the type of
		* @return snuffbox::JSWrapper::Types The type of the value
		*/
		static Types TypeOf(const v8::Local<v8::Value>& value);

		/**
		* @brief Converts a JavaScript type to a string value
		* @param[in] type (const snuffbox::JSWrapper::Types&) The type to convert
		* @return std::string The converted string value
		*/
		static std::string TypeToString(const Types& type);

		/**
		* @brief Constructs an argument error and logs it
		* @param[in] expected (const snuffbox::JSWrapper::Types&) The expected type
		* @param[in] got (const snuffbox::JSWrapper::Types&) The type that was actually received
		* @param[in] arg (const int&) The current argument
		*/
		void Error(const Types& expected, const Types& got, const int& arg);

		/**
		* @brief Checks the argument scope if the format is as it should be
		* @param[in] format (const std::string&) The format to check
		* @return bool Was the format check completed succesfully?
		*/ 
		bool Check(const std::string& format);

		/**
		* @brief Disables error checking in snuffbox::JSWrapper::Check
		* @param[in] value (const bool&) On/off value for error checking
		*/
		void set_error_checks(const bool& value);

	private:
		JS_ARGS args_; //!< The cached argument scope
		bool error_checks_; //!< A boolean for multiple type checking
	};

	//-------------------------------------------------------------------------------------------
	template<>
	inline bool JSWrapper::GetValue<bool>(const int& arg, const bool& def)
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
	inline int JSWrapper::GetValue<int>(const int& arg, const int& def)
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
	inline float JSWrapper::GetValue<float>(const int& arg, const float& def)
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
	inline double JSWrapper::GetValue<double>(const int& arg, const double& def)
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
	inline std::string JSWrapper::GetValue<std::string>(const int& arg, const std::string& def)
	{
		if (args_[arg]->IsNull() || args_[arg]->IsUndefined())
		{
			return def;
		}

		return *v8::String::Utf8Value(args_[arg]->ToString());
	}

  //-------------------------------------------------------------------------------------------
  template<>
  inline v8::Handle<v8::Value> JSWrapper::GetValue<v8::Handle<v8::Value>>(const int& arg, const v8::Handle<v8::Value>& def)
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
  inline T* JSWrapper::GetPointer(const v8::Handle<v8::Value>& val)
  {
    v8::Local<v8::Object> obj = val->ToObject();
		if (obj.IsEmpty() || obj->IsUndefined())
		{
			return nullptr;
		}

    v8::Local<v8::Value> ext = obj->GetHiddenValue(v8::String::NewFromUtf8(JSStateWrapper::Instance()->isolate(), "__ptr"));

		if (ext.IsEmpty() || ext->IsExternal() == false)
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
  inline T* JSWrapper::GetPointer(const int& arg)
  {
    v8::Local<v8::Object> obj = args_[arg]->ToObject();
		if (obj.IsEmpty() || obj->IsUndefined())
		{
			return nullptr;
		}

    v8::Local<v8::Value> ext = obj->GetHiddenValue(v8::String::NewFromUtf8(JSStateWrapper::Instance()->isolate(), "__ptr"));

    if (ext.IsEmpty() || ext->IsExternal() == false)
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
  inline v8::Handle<v8::Value> JSWrapper::CastValue<double>(const double& val)
  {
    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    return v8::Number::New(isolate, val);
  }

  //-------------------------------------------------------------------------------------------
  template<>
  inline v8::Handle<v8::Value> JSWrapper::CastValue<int>(const int& val)
  {
    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    return v8::Number::New(isolate, val);
  }

  //-------------------------------------------------------------------------------------------
  template<>
  inline v8::Handle<v8::Value> JSWrapper::CastValue<float>(const float& val)
  {
    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    return v8::Number::New(isolate, val);
  }

  //-------------------------------------------------------------------------------------------
  template<>
  inline v8::Handle<v8::Value> JSWrapper::CastValue<bool>(const bool& val)
  {
    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    return v8::Boolean::New(isolate, val);
  }

  //-------------------------------------------------------------------------------------------
  template<>
  inline v8::Handle<v8::Value> JSWrapper::CastValue<std::string>(const std::string& val)
  {
    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    return v8::String::NewFromUtf8(isolate, val.c_str());
  }

  //-------------------------------------------------------------------------------------------
  template<typename T>
  inline void JSWrapper::ReturnValue(const T& val)
  {
    args_.GetReturnValue().Set<v8::Value>(JSWrapper::CastValue<T>(val));
  }

  //-------------------------------------------------------------------------------------------
  template<>
  inline void JSWrapper::ReturnValue<v8::Handle<v8::Object>>(const v8::Handle<v8::Object>& val)
  {
    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    args_.GetReturnValue().Set<v8::Object>(val);
  }

	//-------------------------------------------------------------------------------------------
	template<>
	inline void JSWrapper::ReturnValue<v8::Handle<v8::Array>>(const v8::Handle<v8::Array>& val)
	{
		v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
		args_.GetReturnValue().Set<v8::Array>(val);
	}

  //-------------------------------------------------------------------------------------------
  template<>
  inline void JSWrapper::SetObjectValue<double>(const v8::Handle<v8::Object>& obj, const std::string& field, const double& val)
  {
    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    obj->Set(v8::String::NewFromUtf8(isolate, field.c_str()), v8::Number::New(isolate, val));
  }

  //-------------------------------------------------------------------------------------------
  template<>
  inline void JSWrapper::SetObjectValue<float>(const v8::Handle<v8::Object>& obj, const std::string& field, const float& val)
  {
    JSWrapper::SetObjectValue<double>(obj, field, val);
  }

  //-------------------------------------------------------------------------------------------
  template<>
  inline void JSWrapper::SetObjectValue<int>(const v8::Handle<v8::Object>& obj, const std::string& field, const int& val)
  {
    JSWrapper::SetObjectValue<double>(obj, field, val);
  }

  //-------------------------------------------------------------------------------------------
  template<>
  inline void JSWrapper::SetObjectValue<bool>(const v8::Handle<v8::Object>& obj, const std::string& field, const bool& val)
  {
    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    obj->Set(v8::String::NewFromUtf8(isolate, field.c_str()), v8::Boolean::New(isolate, val));
  }

  //-------------------------------------------------------------------------------------------
  template<>
  inline void JSWrapper::SetObjectValue<std::string>(const v8::Handle<v8::Object>& obj, const std::string& field, const std::string& val)
  {
    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    obj->Set(v8::String::NewFromUtf8(isolate, field.c_str()), v8::String::NewFromUtf8(isolate, val.c_str()));
  }

  //-------------------------------------------------------------------------------------------
  template<>
  inline void JSWrapper::SetObjectValue<v8::Handle<v8::Object>>(const v8::Handle<v8::Object>& obj, const std::string& field, const v8::Handle<v8::Object>& val)
  {
    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    obj->Set(v8::String::NewFromUtf8(isolate, field.c_str()), val);
  }

	//-------------------------------------------------------------------------------------------
	template<>
	inline void JSWrapper::SetObjectValue<v8::Handle<v8::Array>>(const v8::Handle<v8::Object>& obj, const std::string& field, const v8::Handle<v8::Array>& val)
	{
		v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
		obj->Set(v8::String::NewFromUtf8(isolate, field.c_str()), val);
	}

	//-------------------------------------------------------------------------------------------
	template<>
	inline void JSWrapper::SetArrayValue<double>(const v8::Handle<v8::Array>& obj, const int& idx, const double& val)
	{
		v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
		obj->Set(idx, v8::Number::New(isolate, val));
	}

	//-------------------------------------------------------------------------------------------
	template<>
	inline void JSWrapper::SetArrayValue<float>(const v8::Handle<v8::Array>& obj, const int& idx, const float& val)
	{
		JSWrapper::SetArrayValue<double>(obj, idx, val);
	}

	//-------------------------------------------------------------------------------------------
	template<>
	inline void JSWrapper::SetArrayValue<int>(const v8::Handle<v8::Array>& obj, const int& idx, const int& val)
	{
		JSWrapper::SetArrayValue<double>(obj, idx, val);
	}

	//-------------------------------------------------------------------------------------------
	template<>
	inline void JSWrapper::SetArrayValue<bool>(const v8::Handle<v8::Array>& obj, const int& idx, const bool& val)
	{
		v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
		obj->Set(idx, v8::Boolean::New(isolate, val));
	}

	//-------------------------------------------------------------------------------------------
	template<>
	inline void JSWrapper::SetArrayValue<std::string>(const v8::Handle<v8::Array>& obj, const int& idx, const std::string& val)
	{
		v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
		obj->Set(idx, v8::String::NewFromUtf8(isolate, val.c_str()));
	}

	//-------------------------------------------------------------------------------------------
	template<>
	inline void JSWrapper::SetArrayValue<v8::Handle<v8::Object>>(const v8::Handle<v8::Array>& obj, const int& idx, const v8::Handle<v8::Object>& val)
	{
		v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
		obj->Set(idx, val);
	}

	//-------------------------------------------------------------------------------------------
	template<>
	inline void JSWrapper::SetArrayValue<v8::Handle<v8::Array>>(const v8::Handle<v8::Array>& obj, const int& idx, const v8::Handle<v8::Array>& val)
	{
		v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
		obj->Set(idx, val);
	}
}