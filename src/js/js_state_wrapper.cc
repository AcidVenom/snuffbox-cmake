#include <libplatform\libplatform.h>

#include "../js/js_state_wrapper.h"

#include "../memory/allocated_memory.h"
#include "../memory/shared_ptr.h"


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

		Isolate* isolate = Isolate::New();
		isolate->Enter();

		Isolate::Scope isolate_scope(isolate);

		HandleScope scope(isolate);

		Handle<ObjectTemplate> global = CreateGlobal();
		global_.Reset(isolate, global);

		Handle<Context> context = CreateContext(global);
		context_.Reset(isolate, context);

		Context::Scope context_scope(context);

		Local<Script> script = Script::Compile(String::NewFromUtf8(isolate, "var x = 0;"), String::NewFromUtf8(isolate, "test.js"));
		Local<Value> result = script->Run();

		SNUFF_LOG_INFO(*String::Utf8Value(result));		
	}

	//-------------------------------------------------------------------------------------------
	Handle<ObjectTemplate> JSStateWrapper::CreateGlobal()
	{
		Isolate* isolate = Isolate::GetCurrent();
		return ObjectTemplate::New(isolate);
	}

	//-------------------------------------------------------------------------------------------
	Handle<Context> JSStateWrapper::CreateContext(Handle<ObjectTemplate> global)
	{
		Isolate* isolate = Isolate::GetCurrent();
		return Context::New(isolate, NULL, global);
	}

	//-------------------------------------------------------------------------------------------
	void JSStateWrapper::Destroy()
	{
		Isolate* isolate = Isolate::GetCurrent();
		
		SNUFF_LOG_DEBUG("Collecting all garbage..");
		isolate->LowMemoryNotification();
		SNUFF_LOG_DEBUG(".. Collected garbage");

		global_.Reset();
		context_.Reset();

		isolate->Exit();
		isolate->Dispose();

		V8::Dispose();

		delete platform_;
		platform_ = nullptr;
	}

	//-------------------------------------------------------------------------------------------
	JSStateWrapper::~JSStateWrapper()
	{
		Destroy();
	}
}