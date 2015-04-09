#include "../cvar/cvar.h"

#include "../memory/allocated_memory.h"
#include "../memory/shared_ptr.h"

#include "../js/js_state_wrapper.h"

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	CVar::CVar()
	{

	}

	//-------------------------------------------------------------------------------------------
	CVar* CVar::Instance()
	{
		static SharedPtr<CVar> cvar = AllocatedMemory::Instance().Construct<CVar>();
		return cvar.get();
	}

	//-------------------------------------------------------------------------------------------
	void CVar::Register(const std::string& name, const bool& value)
	{
		CVar::Boolean* cvar = AllocatedMemory::Instance().Construct<CVar::Boolean>(value);
		Map(name, cvar);
	}

	//-------------------------------------------------------------------------------------------
	void CVar::Register(const std::string& name, const double& value)
	{
		CVar::Number* cvar = AllocatedMemory::Instance().Construct<CVar::Number>(value);
		Map(name, cvar);
	}

	//-------------------------------------------------------------------------------------------
	void CVar::Register(const std::string& name, const std::string& value)
	{
		CVar::String* cvar = AllocatedMemory::Instance().Construct<CVar::String>(value);
		Map(name, cvar);
	}

	//-------------------------------------------------------------------------------------------
	void CVar::Map(const std::string& name, CVar::Value* val)
	{
		CVarMap::iterator it = vars_.find(name);

		if (it != vars_.end())
		{
			AllocatedMemory::Instance().Destruct<CVar::Value>(it->second);
			it->second = val;
			return;
		}

		vars_.emplace(name, val);
	}

	//-------------------------------------------------------------------------------------------
	CVar::Value* CVar::Get(const std::string& name, bool* found)
	{
		CVarMap::const_iterator it = vars_.find(name);
	
		if (it != vars_.end())
		{
			*found = true;
			return it->second;
		}

		*found = false;
		return nullptr;
	}

	//-------------------------------------------------------------------------------------------
	bool CVar::Exists(const std::string& name)
	{
		return vars_.find(name) != vars_.end();
	}

	//-------------------------------------------------------------------------------------------
	void CVar::ParseCommandLine(const int& argc, char** argv)
	{
		std::string arguments = "";

		for (int i = 1; i < argc; ++i)
		{
			arguments += argv[i];
			arguments += " ";
		}

		std::string delimiter = "\\";

		size_t pos = 0;
		std::string token;
		std::string src_directory = "";
		std::string working_directory = argv[0];
		bool atStart = true;

		while ((pos = working_directory.find(delimiter)) != std::string::npos) {
			if (atStart == false)
			{
				src_directory += "/";
			}
			token = working_directory.substr(0, pos);
			working_directory.erase(0, pos + delimiter.length());
			src_directory += token;
			atStart = false;
		}

		Register("src_directory", src_directory);

		int i = 0;
		char ch;

		while (i < arguments.size())
		{
			SkipWhiteSpaces(arguments, i);

			ch = arguments.at(i);

			if (Consume("-", arguments, i) == true)
			{
				SkipWhiteSpaces(arguments, i);
				std::string name = RepeatUntil({" "}, arguments, i);
				SkipWhiteSpaces(arguments, i);
				std::string value = RepeatUntil({" "}, arguments, i);
				
				char* num;
				double number_value = strtod(value.c_str(), &num);

				if (!*num)
				{
					CVar::Register(name, number_value);
				}
				else
				{
					if (strcmp(value.c_str(), "true") == 0 || strcmp(value.c_str(), "false") == 0)
					{
						CVar::Register(name, strcmp(value.c_str(), "true") == 0);
					}
					else
					{
						CVar::Register(name, value);
					}
				}
				--i;
			}

			++i;
		}
	}

	//-------------------------------------------------------------------------------------------
	void CVar::RegisterCommandLine(const int& argc, char** argv)
	{
		ParseCommandLine(argc, argv);
	}

  //-------------------------------------------------------------------------------------------
  void CVar::LogCVars()
  {
    std::string result = "\n\nConsole variables:\n";
    std::string value = "";
    for (CVarMap::iterator it = vars_.begin(); it != vars_.end(); ++it)
    {
      if (it->second->IsNumber())
      {
        value = std::to_string(it->second->As<CVar::Number>()->value());
      }
      else if (it->second->IsBool())
      {
        if (it->second->As<CVar::Boolean>()->value() == true)
        {
          value = "true";
        }
        else
        {
          value = "false";
        }
      }
      else
      {
        value = it->second->As<CVar::String>()->value();
      }

			result += "\n\t" + it->first + " => " + value;
    }

    SNUFF_LOG_INFO(result + "\n");
  }

	//-------------------------------------------------------------------------------------------
	void CVar::SkipWhiteSpaces(const std::string& str, int& i)
	{
		char ch = str.at(i);
		while ((ch == L'\n' || ch == L'\t' || ch == L'\r' || ch == L' ') && i == str.size())
		{
			ch = str.at(i);
			++i;
		}
	}

	//-------------------------------------------------------------------------------------------
	bool CVar::Consume(const std::string& consumer, const std::string& str, int& i)
	{
		char ch = str.at(i);
		int count = 0;

		while (count < consumer.length())
		{
			if (consumer.at(count) != str.at(i + count))
			{
				return false;
			}

			++count;
		}

		i += count;
		return true;
	}

	//-------------------------------------------------------------------------------------------
	std::string CVar::RepeatUntil(const std::vector<std::string>& consumer, const std::string& str, int& i)
	{
		std::string to_return = "";
		bool consumed = false;
		while (consumed == false)
		{
			to_return += str.at(i);
			++i;

			for (int c = 0; c < consumer.size(); ++c)
			{
				consumed = Consume(consumer.at(c), str, i);
			}
		}
		
		return to_return;
	}

	//-------------------------------------------------------------------------------------------
	CVar::~CVar()
	{
		for (CVarMap::iterator it = vars_.begin(); it != vars_.end(); ++it)
		{
			AllocatedMemory::Instance().Destruct<CVar::Value>(it->second);
		}

		vars_.clear();
	}

	//-------------------------------------------------------------------------------------------
	CVar::Value::Value(const ValueTypes& type) :
		type_(type)
	{

	}

	//-------------------------------------------------------------------------------------------
	bool CVar::Value::IsBool()
	{
		return type_ == ValueTypes::kBoolean;
	}

	//-------------------------------------------------------------------------------------------
	bool CVar::Value::IsNumber()
	{
		return type_ == ValueTypes::kNumber;
	}

	//-------------------------------------------------------------------------------------------
	bool CVar::Value::IsString()
	{
		return type_ == ValueTypes::kString;
	}

	//-------------------------------------------------------------------------------------------
	CVar::Value::~Value()
	{

	}

	//-------------------------------------------------------------------------------------------
	CVar::Boolean::Boolean(const bool& value) :
		CVar::Value(CVar::ValueTypes::kBoolean),
		value_(value)
	{

	}

	//-------------------------------------------------------------------------------------------
	const bool& CVar::Boolean::value() const
	{
		return value_;
	}

	//-------------------------------------------------------------------------------------------
	void CVar::Boolean::set_value(const bool& value)
	{
		value_ = value;
	}

	//-------------------------------------------------------------------------------------------
	CVar::Boolean::~Boolean()
	{

	}

	//-------------------------------------------------------------------------------------------
	CVar::Number::Number(const double& value) :
		CVar::Value(CVar::ValueTypes::kNumber),
		value_(value)
	{

	}

	//-------------------------------------------------------------------------------------------
	const double& CVar::Number::value() const
	{
		return value_;
	}

	//-------------------------------------------------------------------------------------------
	void CVar::Number::set_value(const double& value)
	{
		value_ = value;
	}

	//-------------------------------------------------------------------------------------------
	CVar::Number::~Number()
	{

	}

	//-------------------------------------------------------------------------------------------
	CVar::String::String(const std::string& value) :
		CVar::Value(CVar::ValueTypes::kString),
		value_(value)
	{

	}

	//-------------------------------------------------------------------------------------------
	const std::string& CVar::String::value() const
	{
		return value_;
	}

	//-------------------------------------------------------------------------------------------
	void CVar::String::set_value(const std::string& value)
	{
		value_ = value;
	}

	//-------------------------------------------------------------------------------------------
	CVar::String::~String()
	{
		
	}

  //-------------------------------------------------------------------------------------------
  void CVar::RegisterJS(JS_SINGLETON obj)
  {
    JSFunctionRegister funcs[] = {
      { "register", JSRegister },
			{ "exists", JSExists },
			{ "get", JSGet },
			{ "log", JSLog }
    };

		JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
  }

  //-------------------------------------------------------------------------------------------
  void CVar::JSRegister(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    CVar* self = CVar::Instance();

		wrapper.set_error_checks(false);

    if (wrapper.Check("SB"))
    {
      self->Register(wrapper.GetValue<std::string>(0, "undefined"), wrapper.GetValue<bool>(1, false));
    }
    else if (wrapper.Check("SS"))
    {
      self->Register(wrapper.GetValue<std::string>(0, "undefined"), wrapper.GetValue<std::string>(1, "undefined"));
    }
    else if (wrapper.Check("SN"))
    {
      self->Register(wrapper.GetValue<std::string>(0, "undefined"), wrapper.GetValue<double>(1, 0.0));
    }
    else
    {
			wrapper.set_error_checks(true);
			wrapper.Check("SS");
      SNUFF_LOG_ERROR("Could not register CVar!");
    }
  }

	//-------------------------------------------------------------------------------------------
	void CVar::JSExists(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		CVar* self = CVar::Instance();

		if (wrapper.Check("S"))
		{
			wrapper.ReturnValue<bool>(self->Exists(wrapper.GetValue<std::string>(0, "undefined")));
		}
		else
		{
			SNUFF_LOG_ERROR("Could not retrieve CVar, no name was given");
		}
	}

	//-------------------------------------------------------------------------------------------
	void CVar::JSGet(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		CVar* self = CVar::Instance();

		if (wrapper.Check("S"))
		{
			bool found = true;
			std::string name = wrapper.GetValue<std::string>(0, "undefined");
			CVar::Value* val = self->Get(name, &found);

			if (found == false)
			{
				SNUFF_LOG_DEBUG("Could not retrieve CVar, no cvar with name '" + name + "' exists");
				return;
			}
			
			if (val->IsBool())
			{
				wrapper.ReturnValue<bool>(val->As<CVar::Boolean>()->value());
			}
			else if (val->IsNumber())
			{
				wrapper.ReturnValue<double>(val->As<CVar::Number>()->value());
			}
			else if (val->IsString())
			{
				wrapper.ReturnValue<std::string>(val->As<CVar::String>()->value());
			}
		}
		else
		{
			SNUFF_LOG_ERROR("Could not retrieve CVar, no name was given");
		}
	}

	//-------------------------------------------------------------------------------------------
	void CVar::JSLog(JS_ARGS args)
	{
		CVar::Instance()->LogCVars();
	}
}