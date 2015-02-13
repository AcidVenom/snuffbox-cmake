#pragma once

#include <v8.h>
#include <string>

namespace v8
{
	class Platform;
}

namespace snuffbox
{
	/**
	* @class snuffbox::JSStateWrapper
	* @brief Wraps Google's V8 for easier use with different applications
	* @author Daniël Konings
	*/
	class JSStateWrapper
	{
	public:
		/// Default constructor
		JSStateWrapper();

		/**
		* @brief Retrieves the singleton instance of this object
		* @return snuffbox::JSStateWrapper* The pointer of the singleton
		*/
		static JSStateWrapper* Instance();

		/// Initialises the wrapper for use
		void Initialise();
		
		/**
		* @brief Creates the actual global scope
		* @return v8::Handle<v8::ObjectTemplate> The created global object template handle
		*/
		v8::Handle<v8::ObjectTemplate> CreateGlobal();

		/**
		* @brief Creates the actual context
		* @param[in] global (v8::Handle<v8::ObjectTemplate>) A handle to the global scope object template
		* @return v8::Handle<v8::Context> The created context handle
		*/
		v8::Handle<v8::Context> CreateContext(v8::Handle<v8::ObjectTemplate> global);

		/**
		* @brief Compiles JavaScript source code from a file and executes it
		* @param[in] path (std::string) The source code file to compile and run
		*/
		void CompileAndRun(std::string path);

		/**
		* @brief Retrieves the last error from the JavaScript stack
		* @param[in] try_catch (v8::TryCatch*) The try catch object of V8 that contains the error
		* @param[in] buffer (std::string*) A buffer to allocate the error message in
		* @return bool If there was an actual error or not
		*/
		bool GetException(v8::TryCatch* try_catch, std::string* buffer);

		/**
		* @brief Registers a global JavaScript value
		* @param[in] name (std::string) The name of the value to register
		* @param[in] value (v8::Handle<v8::Value>) The value to register
		*/
		void RegisterGlobal(std::string name, v8::Handle<v8::Value> value);

		/**
		* @brief Registers a JavaScript value to an object
		* @param[in] obj (v8::Handle<v8::Object>) The object to register to
		* @param[in] name (std::string) The name of the value to register
		* @param[in] value (v8::Handle<v8::Value>) The value to register
		*/
		void RegisterToObject(v8::Handle<v8::Object> obj, std::string name, v8::Handle<v8::Value> value);

		/// Destroys the state wrapper and disposes V8
		void Destroy();

		/// Default destructor
		~JSStateWrapper();

		/**
		* @brief Retrieves the isolate running on this thread
		* @return v8::Isolate* The pointer to the current isolate
		*/
		v8::Isolate* isolate();

	private:
		v8::Isolate*	isolate_; //!< Used to create the isolated JavaScript state for this instance of the engine
		v8::Persistent<v8::Context> context_; //!< The context we will use for this JavaScript state
		v8::Persistent<v8::ObjectTemplate> global_; //!< The global scope for use with the JavaScript state
		v8::Platform* platform_; //!< The win32 platform

	private:
		/// Registers basic functions (require, assert, etc.)
		void JSRegisterFunctions();

		static void JSRequire(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void JSAssert(const v8::FunctionCallbackInfo<v8::Value>& args);
	};
}