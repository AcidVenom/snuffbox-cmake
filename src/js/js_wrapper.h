#pragma once

#include <v8.h>
#include <string>

#include "../js/js_function_register.h"

#define JS_ARGS const v8::FunctionCallbackInfo<v8::Value>&
#define JS_NAME(name) static const char* js_name(){ return ##name; }
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

		template<typename T>
		T GetValue(int arg, T def);

    template<typename T>
    T* GetPointer(v8::Handle<v8::Value> val);

    template<typename T>
    T* GetPointer(int arg);

		/**
		* @brief Returns the type of a local value
		* @param[in] value (v8::Local<v8::Value>) The value to check the type of
		* @return snuffbox::JSWrapper::Types The type of the value
		*/
		Types TypeOf(v8::Local<v8::Value> value);

		/**
		* @brief Converts a JavaScript type to a string value
		* @param[in] type (snuffbox::JSWrapper::Types) The type to convert
		* @return std::string The converted string value
		*/
		std::string TypeToString(Types type);

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

		if (args_[arg]->IsString() == true)
		{
			return *v8::String::Utf8Value(args_[arg]->ToString());
		}
		return def;
	}

	//-------------------------------------------------------------------------------------------
	template<>
	inline std::string JSWrapper::GetValue<std::string>(int arg, std::string def)
	{
		if (args_[arg]->IsNull() || args_[arg]->IsUndefined())
		{
			return def;
		}

		if (args_[arg]->IsString() == true)
		{
			return *v8::String::Utf8Value(args_[arg]->ToString());
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
}