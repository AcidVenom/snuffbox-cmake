#include "../cvar/cvar.h"

#include "../memory/allocated_memory.h"
#include "../memory/shared_ptr.h"

#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>

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
	void CVar::Register(std::string name, bool value)
	{
		CVar::Boolean* cvar = AllocatedMemory::Instance().Construct<CVar::Boolean>(value);
		Map(name, cvar);
	}

	//-------------------------------------------------------------------------------------------
	void CVar::Register(std::string name, double value)
	{
		CVar::Number* cvar = AllocatedMemory::Instance().Construct<CVar::Number>(value);
		Map(name, cvar);
	}

	//-------------------------------------------------------------------------------------------
	void CVar::Register(std::string name, std::string value)
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
	CVar::Value* CVar::Get(std::string name, bool* found)
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
	bool CVar::Exists(std::string name)
	{
		return vars_.find(name) != vars_.end();
	}

	//-------------------------------------------------------------------------------------------
	void CVar::ParseCommandLine(int argc, char** argv)
	{
		std::string arguments = "";

		for (int i = 0; i < argc; ++i)
		{
			arguments += argv[i];
		}

		std::string delimiter = "\\";

		size_t pos = 0;
		std::string token;
		std::string src_directory;
		while ((pos = arguments.find(delimiter)) != std::string::npos) {
			token = arguments.substr(0, pos);
			arguments.erase(0, pos + delimiter.length());
			src_directory += token + "/";
		}

		Register("src_directory", src_directory);
	}

	//-------------------------------------------------------------------------------------------
	void CVar::RegisterCommandLine(int argc, char** argv)
	{
		ParseCommandLine(argc, argv);
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
	CVar::Value::Value(ValueTypes type) :
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
	CVar::Boolean::Boolean(bool value) :
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
	void CVar::Boolean::set_value(bool value)
	{
		value_ = value;
	}

	//-------------------------------------------------------------------------------------------
	CVar::Boolean::~Boolean()
	{

	}

	//-------------------------------------------------------------------------------------------
	CVar::Number::Number(double value) :
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
	void CVar::Number::set_value(double value)
	{
		value_ = value;
	}

	//-------------------------------------------------------------------------------------------
	CVar::Number::~Number()
	{

	}

	//-------------------------------------------------------------------------------------------
	CVar::String::String(std::string value) :
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
	void CVar::String::set_value(std::string value)
	{
		value_ = value;
	}

	//-------------------------------------------------------------------------------------------
	CVar::String::~String()
	{
		
	}
}