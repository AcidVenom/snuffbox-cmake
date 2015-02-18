#include "../memory/allocated_memory.h"
#include "../memory/shared_ptr.h"

#include "../application/game.h"
#include "../js/js_state_wrapper.h"

#include "../platform/platform_window.h"

#include "../input/keyboard.h"
#include "../input/mouse.h"

#include "../cvar/cvar.h"

#include "../content/content_manager.h"

#include "../platform/platform_file_watch.h"

#ifdef SNUFF_BUILD_CONSOLE
#include <qapplication.h>
#include <qmainwindow.h>
#include "../console/console.h"
#endif

using namespace snuffbox;

int main(int argc, char** argv)
{
#ifdef SNUFF_BUILD_CONSOLE
  QApplication* app = new QApplication(argc, argv);
  app->setApplicationName("Snuffbox Console");

  QMainWindow* console_window = new QMainWindow();
  Console* console = Console::Instance();
#endif

	AllocatedMemory& memory = AllocatedMemory::Instance();

	std::string name = "Snuffbox_";

#ifdef SNUFF_BUILD_OPENGL
	name += "OGL_";
#else
	name += "D3D11_";
#endif

#ifdef _DEBUG
	name += "Debug";
#else
	name += "Release";
#endif

	name += " " + std::to_string(SNUFF_VERSION_MAJOR) + "." + std::to_string(SNUFF_VERSION_MINOR);
	
	CVar* cvar = CVar::Instance();
	cvar->RegisterCommandLine(argc, argv);

	Game* game = Game::Instance();
	
	ContentManager* content_manager = ContentManager::Instance();
	PlatformFileWatch* file_watch = FileWatch::Instance();
  
#ifdef SNUFF_BUILD_CONSOLE
  console->CheckEnabled();
  if (console->enabled() == true)
  {
    console->Initialise(console_window);

    SNUFF_LOG_INFO("Succesfully initialised the console");
  }
#endif

  SNUFF_LOG_INFO(name);
  cvar->LogCVars();
	
	SharedPtr<Window> window = memory.Construct<Window>(SNUFF_WINDOW_CENTERED, SNUFF_WINDOW_CENTERED, 640, 480, name);
  Keyboard* keyboard = Keyboard::Instance();
  Mouse* mouse = Mouse::Instance();

	game->set_window(window.get());
	game->set_keyboard(keyboard);
	game->set_mouse(mouse);

	JSStateWrapper* js_state_wrapper = JSStateWrapper::Instance();
	js_state_wrapper->Initialise();
	js_state_wrapper->CompileAndRun("main.js");

	game->Verify();

	window->Show();

	game->Initialise();

	bool found;
	CVar::Value* reload = cvar->Get("reload", &found);
	bool should_reload = found != false && reload->IsBool() && reload->As<CVar::Boolean>()->value() == true;

	while (game->started())
	{
		if (should_reload)
		{
			file_watch->Update();
		}

		game->Run();
    js_state_wrapper->isolate()->IdleNotification(32);

		file_watch->Process();
	}

	SNUFF_LOG_INFO("Shutting down");
  js_state_wrapper->Destroy();

	return 0;
}