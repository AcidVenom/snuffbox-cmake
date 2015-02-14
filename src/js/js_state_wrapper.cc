#include <algorithm>
#include <libplatform\libplatform.h>

#include "../js/js_state_wrapper.h"

#include "../memory/allocated_memory.h"
#include "../memory/shared_ptr.h"

#include "../platform/platform_text_file.h"
#include "../cvar/cvar.h"

#include "../js/js_wrapper.h"

#include "../js/js_object_register.h"
#include "../js/js_object.h"

using namespace v8;

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	JSStateWrapper::JSStateWrapper() :
		platform_(nullptr)
	{
		
	}

	//-------------------------------------------------------------------------------------------
	JSStateWrapper* JSStateWrapper::Instance()
	{
		static SharedPtr<JSStateWrapper> js_state_wrapper = AllocatedMemory::Instance().Construct<JSStateWrapper>();
		return js_state_wrapper.get();
	}

	//-------------------------------------------------------------------------------------------
	void JSStateWrapper::Initialise()
	{
		V8::InitializeICU();
		V8::Initialize();
		platform_ = platform::CreateDefaultPlatform();
		V8::InitializePlatform(platform_);

		isolate_ = Isolate::New();
		isolate_->Enter();

		Isolate::Scope isolate_scope(isolate_);

		HandleScope scope(isolate_);

		Handle<ObjectTemplate> global = CreateGlobal();
		global_.Reset(isolate_, global);

		Handle<Context> context = CreateContext(global);
		context_.Reset(isolate_, context);

		context->Enter();

		JSRegisterFunctions();
    JSRegister::Register();

		context->Exit();
	}

	//-------------------------------------------------------------------------------------------
	Handle<ObjectTemplate> JSStateWrapper::CreateGlobal()
	{
		return ObjectTemplate::New(isolate_);
	}

	//-------------------------------------------------------------------------------------------
	Handle<Context> JSStateWrapper::CreateContext(Handle<ObjectTemplate> global)
	{
		return Context::New(isolate_, NULL, global);
	}

  //-------------------------------------------------------------------------------------------
  void JSStateWrapper::Run(std::string src, std::string file, bool log)
  {
    HandleScope scope(isolate_);

    Local<Context> context = Local<Context>::New(isolate_, context_);
    Context::Scope context_scope(context);

    TryCatch try_catch;

    Local<Script> script = Script::Compile(String::NewFromUtf8(isolate_, src.c_str()), String::NewFromUtf8(isolate_, file.c_str()));
    Local<Value> result = script->Run();

    if (result.IsEmpty() == true)
    {
      std::string error;
      bool has_error = GetException(&try_catch, &error);

      if (has_error == true)
      {
        SNUFF_LOG_ERROR(error);
      }
      return;
    }
    else
    {
      if (log == true)
      {
        SNUFF_LOG_DEBUG(*String::Utf8Value(result->ToString()));
      }
    }
  }

	//-------------------------------------------------------------------------------------------
	void JSStateWrapper::CompileAndRun(std::string path)
	{
		HandleScope handle_scope(isolate_);

		Local<Context> context = Local<Context>::New(isolate_, context_);
		Context::Scope context_scope(context);

		TextFile file;

		CVar* cvar = CVar::Instance();
		bool found_dir = false;
		CVar::Value* src_directory = cvar->Get("src_directory", &found_dir);

		SNUFF_XASSERT(found_dir == true, "The 'src_directory' CVar could not be found!", "JSStateWrapper::CompileAndRun");
		SNUFF_XASSERT(src_directory != nullptr && src_directory->IsString() == true, "The 'src_directory' CVar is corrupt or is not of a string type!", "JSStateWrapper::CompileAndRun");
		
		std::string fullPath = src_directory->As<CVar::String>()->value() + "/" + path;
		bool success = file.Open(fullPath);

		SNUFF_XASSERT(success == true, "The file '" + path + "' could not be opened!", "JSStateWrapper::CompileAndRun");

    Run(file.Read(), path);
	}

	//-------------------------------------------------------------------------------------------
	bool JSStateWrapper::GetException(TryCatch* try_catch, std::string* buffer)
	{
		HandleScope handle_scope(isolate_);
		String::Utf8Value exception(try_catch->Exception());
		Handle<Message> message = try_catch->Message();

		std::string error = "";

		if (!message.IsEmpty()){
			String::Utf8Value sourceline(message->GetSourceLine());
			error += "\n\n";

			std::string srcline = *sourceline;

			auto it = std::remove_if(std::begin(srcline), std::end(srcline), [](char c){ return (c == '\t'); });
			srcline.erase(it, std::end(srcline));

			error += srcline;
			error += "\n";

			int start = message->GetStartColumn();
			for (int i = 0; i < start; i++) {
				error += " ";
			}
			int end = message->GetEndColumn();
			for (int i = start; i < end; i++) {
				error += "^";
			}
			String::Utf8Value stack_trace(try_catch->StackTrace());

			error += "\n\t";
			error += *stack_trace;
			error += "\n";

			if (stack_trace.length() > 0) {
				*buffer = error;
				return true;
			}
		}

		return false;
	}

	//-------------------------------------------------------------------------------------------
	void JSStateWrapper::RegisterGlobal(std::string name, Handle<Value> value)
	{
		Local<Object> global = Local<Context>::New(isolate_, context_)->Global();
		global->Set(String::NewFromUtf8(isolate_, name.c_str()), value);
	}

	//-------------------------------------------------------------------------------------------
	void JSStateWrapper::RegisterToObject(Handle<Object> obj, std::string name, Handle<Value> value)
	{
		obj->Set(String::NewFromUtf8(isolate_, name.c_str()), value);
	}

	//-------------------------------------------------------------------------------------------
	void JSStateWrapper::Destroy()
	{
		SNUFF_LOG_INFO("Collecting all JavaScript garbage");

		global_.Reset();
		context_.Reset();

    isolate_->LowMemoryNotification();
		isolate_->Exit();
		isolate_->Dispose();

		V8::Dispose();

		delete platform_;
		platform_ = nullptr;
	}

	//-------------------------------------------------------------------------------------------
	v8::Isolate* JSStateWrapper::isolate()
	{
		return isolate_;
	}

	//-------------------------------------------------------------------------------------------
	JSStateWrapper::~JSStateWrapper()
	{
		
	}

	//-------------------------------------------------------------------------------------------
	void JSStateWrapper::JSRegisterFunctions()
	{
		RegisterGlobal("require", Function::New(isolate_, JSRequire));
		RegisterGlobal("assert", Function::New(isolate_, JSAssert));
	}

  //-------------------------------------------------------------------------------------------
  template<typename T>
  void JSStateWrapper::JSNew(JS_ARGS args)
  {
    JSStateWrapper* wrapper = JSStateWrapper::Instance();
    Isolate* isolate = wrapper->isolate();
    T* ptr = AllocatedMemory::Instance().Construct<T>(args);

    Handle<Object> obj = args.This();
    ptr->object().Reset(isolate, obj);
    ptr->object().SetWeak(static_cast<JSObject*>(ptr), JSDestroy);
    ptr->object().MarkIndependent();
    obj->SetHiddenValue(String::NewFromUtf8(isolate, "__ptr"), External::New(isolate, static_cast<void*>(ptr)));
    int64_t size = static_cast<int64_t>(sizeof(ptr));

    isolate->AdjustAmountOfExternalAllocatedMemory(size);
    args.GetReturnValue().Set(obj);
  }

  //-------------------------------------------------------------------------------------------
  void JSStateWrapper::JSDestroy(const v8::WeakCallbackData<v8::Object, JSObject>& data)
  {
    JSObject* ptr = data.GetParameter();

    int64_t size = -static_cast<int64_t>(sizeof(ptr));
    ptr->object().Reset();
    AllocatedMemory::Instance().Destruct<JSObject>(ptr);
    data.GetValue().Clear();
    JSStateWrapper::Instance()->isolate()->AdjustAmountOfExternalAllocatedMemory(size);
  }

	//-------------------------------------------------------------------------------------------
	void JSStateWrapper::JSRequire(JS_ARGS args)
	{
		HandleScope scope(args.GetIsolate());
		JSWrapper wrapper(args);
		bool check = wrapper.Check("S");
		
		if (check == false)
		{
			return;
		}
		else
		{
			JSStateWrapper::Instance()->CompileAndRun(wrapper.GetValue<std::string>(0, "") + ".js");
		}
	}

	//-------------------------------------------------------------------------------------------
	void JSStateWrapper::JSAssert(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		bool check = wrapper.Check("BS");

		SNUFF_XASSERT(check == true ? wrapper.GetValue<bool>(0, false) : false, wrapper.GetValue<std::string>(1, "Assertion failed"), "JSStateWrapper::JSAssert");
	}
}