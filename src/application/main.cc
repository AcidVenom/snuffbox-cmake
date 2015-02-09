#include "../memory/allocated_memory.h"

#include "../application/game.h"

#include "../platform/platform_window.h"

#include "../input/keyboard.h"
#include "../input/mouse.h"

using namespace snuffbox;

int main(int argc, char** argv)
{
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

	Game* game = Game::Instance();
	
	SharedPtr<Window> window = memory.Construct<Window>(SNUFF_WINDOW_CENTERED, SNUFF_WINDOW_CENTERED, 640, 480, name);
	SharedPtr<Keyboard> keyboard = memory.Construct<Keyboard>();
	SharedPtr<Mouse> mouse = memory.Construct<Mouse>();

	game->set_window(window.get());
	game->set_keyboard(keyboard.get());
	game->set_mouse(mouse.get());

	game->Verify();

	window->Show();

	while (game->started())
	{
		game->Run();
	}

	SNUFF_LOG_INFO("Shutting down");
	return 0;
}