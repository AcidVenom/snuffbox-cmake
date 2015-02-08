#pragma once

namespace snuffbox
{
	class Window;

	class Game
	{
	public:
		Game(Window* window);

	private:
		Window* window_;
	};
}