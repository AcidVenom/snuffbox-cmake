#include "../js/js_wrapper.h"

using namespace v8;

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	JSWrapper::JSWrapper(JS_ARGS args) :
		args_(args),
		error_checks_(true)
	{
		
	}

  //------------------------------------------------------------------------------
  Handle<Object> JSWrapper::CreateObject()
  {
    Isolate* isolate = JSStateWrapper::Instance()->isolate();
    return Object::New(isolate);
  }

	//------------------------------------------------------------------------------
	JSWrapper::Types JSWrapper::TypeOf(Local<Value> value)
	{
		if (value->IsFunction())
		{
			return Types::kFunction;
		}
		else if (value->IsArray())
		{
			return Types::kArray;
		}
		else if (value->IsObject())
		{
			return Types::kObject;
		}
		else if (value->IsNumber())
		{
			return Types::kNumber;
		}
		else if (value->IsNull())
		{
			return Types::kNull;
		}
		else if (value->IsBoolean())
		{
			return Types::kBoolean;
		}
		else if (value->IsUndefined())
		{
			return Types::kUndefined;
		}
		else if (value->IsString())
		{
			return Types::kString;
		}
		else
		{
			return Types::kUnknown;
		}
	}

	//-------------------------------------------------------------------------------------------
	std::string JSWrapper::TypeToString(Types type)
	{
		switch (type)
		{
		case Types::kVoid: return "void";
		case Types::kFunction: return "function";
		case Types::kArray: return "array";
		case Types::kObject: return "object";
		case Types::kNumber: return "number";
		case Types::kNull: return "null";
		case Types::kBoolean: return "boolean";
		case Types::kUndefined: return "undefined";
		case Types::kUnknown: return "unknown";
		default: return "unknown";
		}
	}

	//-------------------------------------------------------------------------------------------
	bool JSWrapper::Check(std::string format)
	{
		if (format.size() == 0 || error_checks_ == false)
		{
			return true;
		}

		char f = 'V';
		Types expected, got;
		for (int i = 0; i < format.size(); ++i)
		{
			f = tolower(format.at(i));

			switch (f)
			{
			case 'v':
				expected = Types::kVoid;
				break;
			case 'n':
				expected = Types::kNumber;
				break;
			case 'b':
				expected = Types::kBoolean;
				break;
			case 's':
				expected = Types::kString;
				break;
			case 'o':
				expected = Types::kObject;
				break;
			case 'a':
				expected = Types::kArray;
				break;
			case 'f':
				expected = Types::kFunction;
				break;
			}

			if (expected == Types::kVoid)
			{
				return true;
			}

			got = TypeOf(args_[i]);
			if (got != expected)
			{
				Error(expected, got);
				return false;
			}
		}

		return true;
	}

	//-------------------------------------------------------------------------------------------
	void JSWrapper::Error(JSWrapper::Types expected, JSWrapper::Types got)
	{

	}

	//-------------------------------------------------------------------------------------------
	JSWrapper::~JSWrapper()
	{
	
	}
}