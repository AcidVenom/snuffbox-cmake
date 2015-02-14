#pragma once

#include <v8.h>

#define JS_SINGLETON v8::Handle<v8::Object>
#define JS_CONSTRUCTABLE v8::Handle<v8::ObjectTemplate>

namespace snuffbox
{
	/**
	* @class snuffbox::JSObject
	* @brief The base class of every class that needs to be exposed to JavaScript
	* @author Daniël Konings
	*/
	class JSObject
	{
	public:
    virtual ~JSObject(){}

    /**
    * @return v8::Persistent<v8::Object>& The persistent handle of this object
    */
    v8::Persistent<v8::Object>& object();

  private:
    v8::Persistent<v8::Object> object_;
	};

  //---------------------------------------------------------------------------------------------------------
  inline v8::Persistent<v8::Object>& JSObject::object()
  {
    return object_;
  }
}
