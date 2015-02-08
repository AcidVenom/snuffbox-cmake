#include "../application/game.h"
#include "../platform/platform_window.h"

namespace snuffbox
{
	Game::Game(Window* window)
		: window_(window)
	{
		window_->Initialise();
	}
}