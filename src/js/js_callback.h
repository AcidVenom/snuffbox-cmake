#pragma once

#include "../js/js_wrapper.h"
#include "../application/logging.h"

#include <vector>

namespace snuffbox
{
	/// @todo Make more hacks like this
	static void JSWeak(const v8::WeakCallbackData<v8::Function, JSObject>& data)
	{
		
	}

  /**
  * @class snuffbox::JSCallback<...Args>
  * @brief Used to wrap callbacks to JavaScript
  * @author Daniël Konings
  */
	template <typename ...Args>
  class JSCallback
  {
  public:
    /// Default constructor
    JSCallback();

    /**
    * @brief Sets the callback to a field in the global object
    * @param[in] cb (const std::string&) The name of the field to assign to the callback
    * @return bool Was setting the callback a success? Fails if; undefined or not of a function type
    */
    bool Set(const std::string& cb);

    /**
    * @brief Sets the callback to a field of an object in the global object
    * @param[in] obj (const std::string&) The name of the object to retrieve from the global object
    * @param[in] field (const std::string&) The name of the field in the object to assign to the callback
    * @return bool Was setting the callback a success? Fails if; object/field is undefined or field is not of a function type
    */
		bool Set(const std::string& obj, const std::string& field);

    /**
    * @brief Sets the callback from a provided function
    * @param[in] cb (const v8::Handle<v8::Value>&) The callback to set
    */
    void Set(const v8::Handle<v8::Value>& cb);

    /**
    * @brief Pushes a value into the value array, to pass to the call of the callback
    * @param[in] arg (const T&) The argument to push
    */
    template <typename T>
    void push(const T& arg);

    /**
    * @brief The last function to break recursion of argument pushing
    * @return int 0, as there are no more arguments to push
    */
    int push_arg();

    /**
    * @brief Pushes an argument when there is only one argument left
    * @param[in] first (const T&) The argument that's left to be pushed
    * @return int 1 Still arguments on the stack
    */
    template <typename T>
		int push_arg(const T& first);

    /**
    * @brief Pushes an argument when there are 2 or more arguments left
    * @param[in] first (const T&) The argument that's next to be pushed
    * @param[in] args (const Others&...) The rest of the arguments that are left to be pushed
    * @return int 1 + previous argument count, Still arguments on the stack
    */
    template <typename T, typename ... Others>
		int push_arg(const T& first, const Others&...args);

    /**
    * @brief Calls the function JavaScript sided
    * @param[in] args (const Args&...) The arguments to forward to JavaScript
    */
    void Call(const Args& ...args);

    /// Default destructor
    ~JSCallback();
  private:
    v8::Persistent<v8::Function> callback_; //!< A persistent handle containing the callback if any
    std::vector<v8::Handle<v8::Value>> values_; //!< The arguments to call with JavaScript sided
    bool valid_; //!< Is this callback valid?
  };

  //-------------------------------------------------------------------------------------------
  template<typename ... Args>
  inline JSCallback<Args...>::JSCallback() :
    valid_(false)
  {

  }

  //-------------------------------------------------------------------------------------------
  template<typename ... Args>
  inline bool JSCallback<Args...>::Set(const std::string& cb)
  {
    JSStateWrapper* wrapper = JSStateWrapper::Instance();
    v8::Isolate* isolate = wrapper->isolate();
    v8::HandleScope scope(isolate);

    v8::Local<v8::Object> global = wrapper->global();
    v8::Handle<v8::Value> value = global->Get(v8::String::NewFromUtf8(isolate, cb.c_str()));

    if (value.IsEmpty() == true || value->IsUndefined())
    {
      SNUFF_LOG_ERROR("Could not find value '" + cb + "'");
      return false;
    }

    if (value->IsFunction() == false)
    {
      SNUFF_LOG_ERROR("Callback with name '" + cb + "' is not of a function type");
      return false;
    }

    callback_.Reset(isolate, value.As<v8::Function>());
		JSObject* null = nullptr;
		callback_.SetWeak(static_cast<JSObject*>(null), JSWeak);
    valid_ = true;

    return true;
  }

  //-------------------------------------------------------------------------------------------
  template<typename ... Args>
	inline bool JSCallback<Args...>::Set(const std::string& obj, const std::string& field)
  {
		callback_.Reset();
    JSStateWrapper* wrapper = JSStateWrapper::Instance();
    v8::Isolate* isolate = wrapper->isolate();
    v8::HandleScope scope(isolate);

    v8::Local<v8::Object> global = wrapper->global();
    v8::Handle<v8::Value> object = global->Get(v8::String::NewFromUtf8(isolate, obj.c_str()));

    if (object.IsEmpty() == true || object->IsUndefined())
    {
      SNUFF_LOG_ERROR("Could not find object '" + obj + "'");
      return false;
    }

    if (object->IsObject() == false)
    {
      SNUFF_LOG_ERROR("Value with name '" + obj + "' is not of an object type");
      return false;
    }

    v8::Handle<v8::Value> value = object->ToObject()->Get(v8::String::NewFromUtf8(isolate, field.c_str()));

    if (value.IsEmpty() == true || value->IsUndefined())
    {
      SNUFF_LOG_ERROR("Could not find callback '" + obj + "." + field + "'");
      return false;
    }

    if (value->IsFunction() == false)
    {
      SNUFF_LOG_ERROR("Callback '" + obj + "." + field + "' is not of a function type");
      return false;
    }

		callback_.Reset(isolate, value.As<v8::Function>());
		JSObject* null = nullptr;
		callback_.SetWeak(static_cast<JSObject*>(null), JSWeak);
    valid_ = true;

    return true;
  }

  //-------------------------------------------------------------------------------------------
  template<typename ... Args>
  inline void JSCallback<Args...>::Set(const v8::Handle<v8::Value>& cb)
  {
    v8::Handle<v8::Value> value = cb;
		callback_.Reset(JSStateWrapper::Instance()->isolate(), value.As<v8::Function>());
		JSObject* null = nullptr;
		callback_.SetWeak(static_cast<JSObject*>(null), JSWeak);
    valid_ = true;
  }

  //-------------------------------------------------------------------------------------------
  template<typename ... Args> template<typename T>
  inline void JSCallback<Args...>::push(const T& arg)
  {
    values_.push_back(JSWrapper::CastValue<T>(arg));
  }

  //-------------------------------------------------------------------------------------------
  template <typename ... Args>
  inline int JSCallback<Args...>::push_arg()
  {
    return 0;
  }

  //-------------------------------------------------------------------------------------------
  template <typename ... Args> template <typename T>
  inline int JSCallback<Args...>::push_arg(const T& first)
  {
    push(first);
    return 1;
  }

  //-------------------------------------------------------------------------------------------
  template <typename ... Args> template <typename T, typename ... Others>
  inline int JSCallback<Args...>::push_arg(const T& first, const Others&...args)
  {
    push(first);
    return push_arg(args...) + 1;
  }

  //-------------------------------------------------------------------------------------------
  template <typename ...Args>
  inline void JSCallback<Args...>::Call(const Args& ...args)
  {
    if (valid_ == false)
    {
      return;
    }

    JSStateWrapper* wrapper = JSStateWrapper::Instance();
    v8::Isolate* isolate = wrapper->isolate();
    v8::HandleScope scope(isolate);

    v8::TryCatch try_catch;

    values_.clear();
    int nargs = push_arg(args...);

    v8::Local<v8::Function> func = v8::Local<v8::Function>::New(isolate, callback_);
		v8::Local<v8::Value> ctx = func->Get(v8::String::NewFromUtf8(isolate, "ctx"));

    nargs != 0 ? func->Call(
			ctx->IsUndefined() == false && ctx->IsObject() == true ? 
			ctx->ToObject() : wrapper->global(), nargs, &values_[0]) 
			
			: 
			
			func->Call(
			ctx->IsUndefined() == false && ctx->IsObject() == true ?
			ctx->ToObject() : wrapper->global(), 0, 0);

    std::string exception;
    bool failed = wrapper->GetException(&try_catch, &exception);

    if (failed == true)
    {
      SNUFF_LOG_ERROR(exception);
    }
  }

  //-------------------------------------------------------------------------------------------
  template<typename ... Args>
  inline JSCallback<Args...>::~JSCallback()
  {
		callback_.Reset();
  }
}
