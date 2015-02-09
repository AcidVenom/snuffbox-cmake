#include "../application/game.h"
#include "../application/logging.h"

#include "../platform/platform_window.h"

#include "../input/keyboard.h"
#include "../input/mouse.h"

#include "../memory/allocated_memory.h"
#include "../memory/shared_ptr.h"

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	Game::Game() : 
		window_(nullptr),
		keyboard_(nullptr),
		mouse_(nullptr),
		started_(true)
	{
		SNUFF_LOG_INFO("Created an instance of the engine");
	}

	//-------------------------------------------------------------------------------------------
	Game* Game::Instance()
	{
		static SharedPtr<Game> game = AllocatedMemory::Instance().Construct<Game>();
		return game.get();
	}

	//-------------------------------------------------------------------------------------------
	void Game::Verify()
	{
		SNUFF_ASSERT_NOTNULL(window_, "Game::Verify::Window");
		SNUFF_ASSERT_NOTNULL(keyboard_, "Game::Verify::Keyboard");
		SNUFF_ASSERT_NOTNULL(mouse_, "Game::Verify::Mouse");
	}

	//-------------------------------------------------------------------------------------------
	void Game::Run()
	{
		window_->ProcessMessages();
		keyboard_->Update();
		mouse_->Update();
	}

	//-------------------------------------------------------------------------------------------
	void Game::Notify(Game::GameNotifications evt)
	{
		switch (evt)
		{
		case Game::GameNotifications::kReload:

			break;
		case Game::GameNotifications::kQuit:
			Quit();
			break;
		}
	}

	//-------------------------------------------------------------------------------------------
	void Game::Quit()
	{
		SNUFF_LOG_INFO("Received quit message");
		started_ = false;
	}

	//-------------------------------------------------------------------------------------------
	const bool& Game::started() const
	{
		return started_;
	}

	//-------------------------------------------------------------------------------------------
	Window* Game::window()
	{
		return window_;
	}

	//-------------------------------------------------------------------------------------------
	Keyboard* Game::keyboard()
	{
		return keyboard_;
	}

	//-------------------------------------------------------------------------------------------
	Mouse* Game::mouse()
	{
		return mouse_;
	}

	//-------------------------------------------------------------------------------------------
	void Game::set_window(Window* window)
	{
		SNUFF_ASSERT_NOTNULL(window, "Game::set_window");
		window_ = window;
	}

	//-------------------------------------------------------------------------------------------
	void Game::set_keyboard(Keyboard* keyboard)
	{
		SNUFF_ASSERT_NOTNULL(keyboard, "Game::set_keyboard");
		keyboard_ = keyboard;
	}

	//-------------------------------------------------------------------------------------------
	void Game::set_mouse(Mouse* mouse)
	{
		SNUFF_ASSERT_NOTNULL(mouse, "Game::set_mouse");
		mouse_ = mouse;
	}

	//-------------------------------------------------------------------------------------------
	Game::~Game()
	{

	}
}