#include "../application/game.h"
#include "../application/logging.h"

#include "../platform/platform_window.h"

#include "../input/keyboard.h"
#include "../input/mouse.h"

#include "../memory/allocated_memory.h"
#include "../memory/shared_ptr.h"

#include "../js/js_callback.h"

#undef min

#ifdef SNUFF_BUILD_CONSOLE
#include "../console/console.h"
#endif

#include <chrono>

using namespace std::chrono;

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	Game::Game() : 
		window_(nullptr),
		keyboard_(nullptr),
		mouse_(nullptr),
		started_(true),
		delta_time_(0.0),
		fixed_step_(16.67),
		left_over_delta_(0.0),
		accumulated_time_(0.0),
		time_(0.0)
	{
    
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

    js_init_.Set("Game", "Initialise");
    js_update_.Set("Game", "Update");
		js_fixed_update_.Set("Game", "FixedUpdate");
		js_shutdown_.Set("Game", "Shutdown");
		js_on_reload_.Set("Game", "OnReload");
	}

	//-------------------------------------------------------------------------------------------
	void Game::Initialise()
	{
		js_init_.Call();
	}

	//-------------------------------------------------------------------------------------------
	void Game::UpdateInput()
	{
		window_->ProcessMessages();
		keyboard_->Update();
		mouse_->Update();
	}

	//-------------------------------------------------------------------------------------------
	void Game::Update()
	{
		js_update_.Call(delta_time_);

		FixedUpdate();
	}

	//-------------------------------------------------------------------------------------------
	void Game::FixedUpdate()
	{
		accumulated_time_ += delta_time_ * 1000;
		int time_steps = 0;
		double fixed_delta = 1000.0f / fixed_step_;

		accumulated_time_ = std::min(accumulated_time_, static_cast<double>(fixed_delta * 2));

		while (accumulated_time_ > fixed_delta)
		{
			++time_steps;
			js_fixed_update_.Call(time_steps, fixed_delta);

			accumulated_time_ -= fixed_delta;
		}
	}

	//-------------------------------------------------------------------------------------------
	void Game::UpdateConsole()
	{
#ifdef SNUFF_BUILD_CONSOLE
		if (Console::Instance()->enabled())
		{
			qApp->processEvents();
		}
#endif
	}

	//-------------------------------------------------------------------------------------------
	void Game::Run()
	{
		high_resolution_clock::time_point last_time = high_resolution_clock::now();

		UpdateInput();
		Update();
		UpdateConsole();

		high_resolution_clock::time_point now = high_resolution_clock::now();

		duration<double, std::milli> dt_duration = duration_cast<duration<double, std::milli>>(now - last_time);
		delta_time_ = dt_duration.count() * 1e-3f;
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
	const double& Game::fixed_step() const
	{
		return fixed_step_;
	}

	//-------------------------------------------------------------------------------------------
	const double& Game::time() const
	{
		return time_;
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
	void Game::set_fixed_step(double step)
	{
		fixed_step_ = step;
	}

	//-------------------------------------------------------------------------------------------
	void Game::set_time(double time)
	{
		time_ = time;
	}

	//-------------------------------------------------------------------------------------------
	Game::~Game()
	{

	}

	//-------------------------------------------------------------------------------------------
	void Game::RegisterJS(JS_SINGLETON obj)
	{
		JSFunctionRegister funcs[] = {
			{ "quit", JSQuit },
			{ "time", JSTime },
			{ "setTime", JSSetTime },
			{ "fixedStep", JSFixedStep },
			{ "setFixedStep", JSSetFixedStep }
		};
		
		JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
	}

	//-------------------------------------------------------------------------------------------
	void Game::JSQuit(JS_ARGS args)
	{
		Game::Instance()->Notify(GameNotifications::kQuit);
	}

	//-------------------------------------------------------------------------------------------
	void Game::JSTime(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		wrapper.ReturnValue<double>(Game::Instance()->time());
	}

	//-------------------------------------------------------------------------------------------
	void Game::JSSetTime(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		wrapper.Check("N");
		Game::Instance()->set_time(wrapper.GetValue<double>(0, 0.0));
	}

	//-------------------------------------------------------------------------------------------
	void Game::JSFixedStep(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		wrapper.ReturnValue<double>(Game::Instance()->fixed_step());
	}

	//-------------------------------------------------------------------------------------------
	void Game::JSSetFixedStep(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		wrapper.Check("N");
		Game::Instance()->set_fixed_step(wrapper.GetValue<double>(0, 0.0));
	}
}