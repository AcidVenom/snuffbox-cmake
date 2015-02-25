#include "../js/js_wrapper.h"
#include "../application/logging.h"

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
	JSWrapper::Types JSWrapper::TypeOf(const Local<Value>& value)
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
	std::string JSWrapper::TypeToString(const Types& type)
	{
		switch (type)
		{
		case Types::kVoid: return "void";
		case Types::kString: return "string";
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
	bool JSWrapper::Check(const std::string& format)
	{
		if (format.size() == 0)
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
				Error(expected, got, i);
				return false;
			}
		}

		return true;
	}

	//-------------------------------------------------------------------------------------------
	void JSWrapper::Error(const JSWrapper::Types& expected, const JSWrapper::Types& got, const int& arg)
	{
		if (error_checks_ == false)
		{
			return;
		}
		Isolate* isolate = JSStateWrapper::Instance()->isolate();
		std::string error = "(";

		error += *String::Utf8Value(args_.This()->ToString());
		error += ".";
		error += *String::Utf8Value(args_.Callee()->GetName()->ToString());
		error += ") ";

		error += "Expected '" + TypeToString(expected) + "', but got '" + TypeToString(got) + "' for argument " + std::to_string(arg + 1) + "\n\t";
		Local<StackTrace> stack = StackTrace::CurrentStackTrace(isolate, 20);

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

		SNUFF_LOG_ERROR(error);
	}

	//-------------------------------------------------------------------------------------------
	void JSWrapper::set_error_checks(const bool& value)
	{
		error_checks_ = value;
	}

	//-------------------------------------------------------------------------------------------
	JSWrapper::~JSWrapper()
	{
	
	}
}