#include "../io/io_manager.h"
#include "../application/game.h"
#include "../platform/platform_text_file.h"
#include "../memory/shared_ptr.h"

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	IOManager::IOManager()
	{

	}

	//-------------------------------------------------------------------------------------------
	IOManager* IOManager::Instance()
	{
		SharedPtr<IOManager> io_manager = AllocatedMemory::Instance().Construct<IOManager>();
		return io_manager.get();
	}

	//-------------------------------------------------------------------------------------------
	bool IOManager::Read(const std::string& path, std::string* buffer)
	{
		TextFile file;
		bool success = file.Open(Game::Instance()->path() + "/" + path);

		if (success == false)
		{
			SNUFF_LOG_ERROR("Could not open file '" + path + "'");
			return false;
		}

		*buffer = file.Read();
		return true;
	}

	//-------------------------------------------------------------------------------------------
	bool IOManager::Exists(const std::string& path)
	{
		TextFile file;
		return file.Open(Game::Instance()->path() + "/" + path);
	}

	//-------------------------------------------------------------------------------------------
	bool IOManager::Write(const std::string& path, const std::string& src)
	{
		TextFile file;
		return file.Write(Game::Instance()->path() + "/" + path, src);
	}

	//-------------------------------------------------------------------------------------------
	bool IOManager::DirectoryExists(const std::string& path)
	{
		DWORD dir = GetFileAttributesA((Game::Instance()->path() + "/" + path).c_str());

		if (dir != INVALID_FILE_ATTRIBUTES && dir & FILE_ATTRIBUTE_DIRECTORY)
		{
			return true;
		}

		return false;
	}

	//-------------------------------------------------------------------------------------------
	void IOManager::CreateDir(const std::string& path)
	{
		if (DirectoryExists(path) == true)
		{
			SNUFF_LOG_WARNING("Attempted to create directory '" + path + "', but it already exists");
			return;
		}

		CreateDirectoryA((Game::Instance()->path() + "/" + path).c_str(), 0);
	}

	//-------------------------------------------------------------------------------------------
	IOManager::~IOManager()
	{

	}

	//-------------------------------------------------------------------------------------------
	void IOManager::RegisterJS(JS_SINGLETON obj)
	{
		JSFunctionRegister funcs[] = {
			{ "read", JSRead },
			{ "exists", JSExists },
			{ "write", JSWrite },
			{ "directoryExists", JSDirectoryExists },
			{ "createDirectory", JSCreateDir }
		};

		JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
	}

	//-------------------------------------------------------------------------------------------
	void IOManager::JSRead(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		
		if (wrapper.Check("S") == true)
		{
			std::string buffer;
			bool success = IOManager::Instance()->Read(wrapper.GetValue<std::string>(0, "undefined"), &buffer);

			if (success == true)
			{
				wrapper.ReturnValue<std::string>(buffer);
			}

			return;
		}

		SNUFF_LOG_ERROR("Unspecified path while trying to read a file");
	}

	//-------------------------------------------------------------------------------------------
	void IOManager::JSExists(JS_ARGS args)
	{
		JSWrapper wrapper(args);

		if (wrapper.Check("S") == true)
		{
			wrapper.ReturnValue<bool>(IOManager::Instance()->Exists(wrapper.GetValue<std::string>(0, "undefined")));
			return;
		}

		SNUFF_LOG_ERROR("Unspecified path while trying to check if a file exists");
	}

	//-------------------------------------------------------------------------------------------
	void IOManager::JSWrite(JS_ARGS args)
	{
		JSWrapper wrapper(args);

		if (wrapper.Check("SS") == true)
		{
			std::string path = wrapper.GetValue<std::string>(0, "undefined");
			bool success = IOManager::Instance()->Write(path, wrapper.GetValue<std::string>(1, "undefined"));
			
			if (success == false)
			{
				SNUFF_LOG_ERROR("Could not write to location '" + path + "'");
				return;
			}
			
			SNUFF_LOG_INFO("Saved '" + path + "'");
		}
	}

	//-------------------------------------------------------------------------------------------
	void IOManager::JSDirectoryExists(JS_ARGS args)
	{
		JSWrapper wrapper(args);

		if (wrapper.Check("S") == true)
		{
			wrapper.ReturnValue<bool>(IOManager::Instance()->DirectoryExists(wrapper.GetValue<std::string>(0, "undefined")));
		}
	}

	//-------------------------------------------------------------------------------------------
	void IOManager::JSCreateDir(JS_ARGS args)
	{
		JSWrapper wrapper(args);

		if (wrapper.Check("S") == true)
		{
			IOManager::Instance()->CreateDir(wrapper.GetValue<std::string>(0, "undefined"));
		}
	}
}