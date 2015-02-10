#include <algorithm>
#include <libplatform\libplatform.h>

#include "../js/js_state_wrapper.h"

#include "../memory/allocated_memory.h"
#include "../memory/shared_ptr.h"

#include "../platform/platform_text_file.h"
#include "../cvar/cvar.h"


using namespace v8;

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	JSStateWrapper::JSStateWrapper() :
		platform_(nullptr)
	{
		Initialise();
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

		CompileAndRun("main.js");
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

		TryCatch try_catch;

		Local<Script> script = Script::Compile(String::NewFromUtf8(isolate_, file.Read().c_str()), String::NewFromUtf8(isolate_, path.c_str()));
		Local<Value> result = script->Run();

		if (result.IsEmpty() == true)
		{
			bool failed = false;
			std::string exception(GetException(&try_catch, &failed));

			if (failed == true)
			{
				SNUFF_LOG_ERROR(exception);
			}
			return;
		}
		else
		{

		}
	}

	//-------------------------------------------------------------------------------------------
	std::string JSStateWrapper::GetException(TryCatch* try_catch, bool* failed)
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
				*failed = true;
				return error;
			}
		}

		return std::string("");
	}

	//-------------------------------------------------------------------------------------------
	void JSStateWrapper::Destroy()
	{
		SNUFF_LOG_DEBUG("Collecting all garbage..");
		isolate_->LowMemoryNotification();
		SNUFF_LOG_DEBUG(".. Collected garbage");

		global_.Reset();
		context_.Reset();

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
		Destroy();
	}
}