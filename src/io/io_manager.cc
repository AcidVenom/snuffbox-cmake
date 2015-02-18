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
	bool IOManager::Read(std::string path, std::string* buffer)
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
	bool IOManager::Exists(std::string path)
	{
		TextFile file;
		return file.Open(Game::Instance()->path() + "/" + path);
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
			{ "exists", JSExists }
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
}