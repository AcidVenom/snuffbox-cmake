#include <algorithm>
#include <libplatform\libplatform.h>

#include "../application/logging.h"
#include "../application/game.h"

#include "../js/js_state_wrapper.h"

#include "../platform/platform_text_file.h"
#include "../cvar/cvar.h"

#include "../js/js_wrapper.h"

#include "../js/js_object_register.h"
#include "../js/js_object.h"

#include "../content/content_manager.h"

#include "../memory/shared_ptr.h"

using namespace v8;

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	static bool stack_dump_available_ = false;

	//-------------------------------------------------------------------------------------------
  JSStateWrapper::JSStateWrapper() :
    platform_(nullptr),
    running_(false)
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

    running_ = true;

		SNUFF_LOG_SUCCESS("Succesfully initialised the JavaScript state");
	}

	//-------------------------------------------------------------------------------------------
	Handle<ObjectTemplate> JSStateWrapper::CreateGlobal()
	{
		return ObjectTemplate::New(isolate_);
	}

	//-------------------------------------------------------------------------------------------
	Handle<Context> JSStateWrapper::CreateContext(const Handle<ObjectTemplate>& global)
	{
		return Context::New(isolate_, NULL, global);
	}

  //-------------------------------------------------------------------------------------------
  void JSStateWrapper::Run(const std::string& src, const std::string& file, const bool& log)
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
	void JSStateWrapper::CompileAndRun(const std::string& path, const bool& reloading)
	{
		HandleScope handle_scope(isolate_);

		Local<Context> context = Local<Context>::New(isolate_, context_);
		Context::Scope context_scope(context);

		TextFile file;

		std::string full_path = Game::Instance()->path() + "/" + path;
		bool success = file.Open(full_path);

		SNUFF_XASSERT(success == true, "The file '" + path + "' could not be opened!", "JSStateWrapper::CompileAndRun");

    Run(file.Read(), path);

		if (reloading == true)
		{
			return;
		}
		ContentManager::Instance()->Notify(ContentManager::Events::kLoad, ContentTypes::kScript, path);
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
	std::string JSStateWrapper::StackDump(const int& max)
	{
		HandleScope scope(isolate_);
		std::string error;
		Local<StackTrace> stack = StackTrace::CurrentStackTrace(isolate_, max);

		Local<StackFrame> frame;
		for (int i = 0; i < stack->GetFrameCount(); ++i)
		{
			frame = stack->GetFrame(i);
			error += "\n\t";
			error += "at " + std::string(*String::Utf8Value(frame->GetFunctionName())) +
				" (" + std::string(*String::Utf8Value(frame->GetScriptName())) +
				":" + std::to_string(frame->GetLineNumber()) + ":" + std::to_string(frame->GetColumn()) + ")";
		}

		error += "\n";

		return error;
	}

	//-------------------------------------------------------------------------------------------
	void JSStateWrapper::RegisterGlobal(const std::string& name, const Handle<Value>& value)
	{
		Local<Object> global = Local<Context>::New(isolate_, context_)->Global();
		global->Set(String::NewFromUtf8(isolate_, name.c_str()), value);
	}

	//-------------------------------------------------------------------------------------------
	void JSStateWrapper::RegisterToObject(const Handle<Object>& obj, const std::string& name, const Handle<Value>& value)
	{
		obj->Set(String::NewFromUtf8(isolate_, name.c_str()), value);
	}

	//-------------------------------------------------------------------------------------------
	void JSStateWrapper::Destroy()
	{
		SNUFF_LOG_INFO("Collecting all JavaScript garbage");

		//SNUFF_LOG_WARNING("Some handle scope");
		HandleScope scope(isolate_);
		//SNUFF_LOG_WARNING("That's fine, let's move on..");

		//SNUFF_LOG_WARNING("This might go wrong..");
		Local<Object> g = global();
		//SNUFF_LOG_WARNING("But it didn't.");
		
		//SNUFF_LOG_WARNING("Retrieving property names");
		for (unsigned int i = 0; i < g->GetPropertyNames()->Length(); ++i)
		{
			//SNUFF_LOG_WARNING("Undefining that bitch named " + std::string(*String::Utf8Value(g->GetPropertyNames()->Get(i))));
			g->Set(g->GetPropertyNames()->Get(i), v8::Undefined(isolate_));
		}
		//SNUFF_LOG_WARNING("Well that worked too");

		SNUFF_LOG_INFO("Collected all JavaScript garbage");
	}

  //-------------------------------------------------------------------------------------------
  void JSStateWrapper::Dispose()
  {
		stack_dump_available_ = false;
		SNUFF_LOG_INFO("Disposing V8 and its state");

    Destroy();

		//SNUFF_LOG_WARNING("This is where it really happens, collecting dat_garbage");
		//SNUFF_LOG_WARNING("Address: 0x" + std::to_string(reinterpret_cast<int>(&isolate_)));
    isolate_->LowMemoryNotification();
		//SNUFF_LOG_WARNING("Hooray");

		//SNUFF_LOG_WARNING("Exiting that damned isolate");
    isolate_->Exit();
		//SNUFF_LOG_WARNING("Seems like that was okay to do");

		//SNUFF_LOG_WARNING("Disposing the isolate, because yeah.");
    isolate_->Dispose();
		//SNUFF_LOG_WARNING("Man, we got far, but..");

		//SNUFF_LOG_WARNING("Can we get further than that?");
    V8::Dispose();
		//SNUFF_LOG_WARNING("Stars and beyond were reached, houston we have no problems.");

		//SNUFF_LOG_WARNING("If this goes wrong I'm eating my toes");
    delete platform_;
    platform_ = nullptr;
		//SNUFF_LOG_WARNING("Lovely toes.");

    running_ = false;

		SNUFF_LOG_INFO("Disposed V8 and its state");
  }

	//-------------------------------------------------------------------------------------------
	Isolate* JSStateWrapper::isolate()
	{
		return isolate_;
	}

  //-------------------------------------------------------------------------------------------
  Local<Object> JSStateWrapper::global()
  {
    return Local<Context>::New(isolate_, context_)->Global();
  }

  //-------------------------------------------------------------------------------------------
  const bool& JSStateWrapper::running() const
  {
    return running_;
  }

	//-------------------------------------------------------------------------------------------
	std::map<std::string, bool>& JSStateWrapper::required()
	{
		return required_;
	}

	//-------------------------------------------------------------------------------------------
	JSStateWrapper::~JSStateWrapper()
	{
    
	}

	//-------------------------------------------------------------------------------------------
	void JSStateWrapper::OpenStack()
	{
		stack_dump_available_ = true;
	}

	//-------------------------------------------------------------------------------------------
	bool JSStateWrapper::StackDumpAvailable()
	{
		return stack_dump_available_;
	}

	//-------------------------------------------------------------------------------------------
	void JSStateWrapper::JSRegisterFunctions()
	{
		RegisterGlobal("require", Function::New(isolate_, JSRequire));
		RegisterGlobal("assert", Function::New(isolate_, JSAssert));
	}

  //-------------------------------------------------------------------------------------------
  void JSStateWrapper::JSDestroy(const v8::WeakCallbackData<v8::Object, JSObject>& data)
  {
    JSObject* ptr = data.GetParameter();

    int64_t size = -static_cast<int64_t>(sizeof(ptr));
		ptr->object().ClearWeak();
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

		JSStateWrapper* instance = JSStateWrapper::Instance();
		
		if (check == false)
		{
			return;
		}
		else
		{
			std::string path = wrapper.GetValue<std::string>(0, "");
			std::map<std::string, bool>& required = instance->required();

			if (required.find(path) != required.end())
			{
				return;
			}

			instance->CompileAndRun(path + ".js");
			required.emplace(path, true);
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