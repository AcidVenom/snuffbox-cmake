#pragma once

#include <v8.h>

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

		/// Destroys the state wrapper and disposes V8
		void Destroy();

		/// Default destructor
		~JSStateWrapper();

	private:
		v8::Isolate*	isolate_; //!< Used to create the isolated JavaScript state for this instance of the engine
		v8::Persistent<v8::Context> context_; //!< The context we will use for this JavaScript state
		v8::Persistent<v8::ObjectTemplate> global_; //!< The global scope for use with the JavaScript state
		v8::Platform* platform_; //!< The win32 platform
	};
}