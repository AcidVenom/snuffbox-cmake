#include "../application/game.h"
#include "../application/logging.h"

#include "../platform/platform_window.h"

#include "../input/keyboard.h"
#include "../input/mouse.h"

#include "../memory/allocated_memory.h"
#include "../memory/shared_ptr.h"

#include "../js/js_callback.h"
#include "../cvar/cvar.h"

#include "../platform/platform_file_watch.h"
#include "../platform/platform_render_device.h"

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
		render_device_(nullptr),
		started_(true),
		delta_time_(0.0),
		fixed_step_(16.67),
		left_over_delta_(0.0),
		accumulated_time_(0.0),
		time_(0.0)
	{
		CVar* cvar = CVar::Instance();
		bool found_dir = false;
		CVar::Value* src_directory = cvar->Get("src_directory", &found_dir);

		SNUFF_XASSERT(found_dir == true, "The 'src_directory' CVar could not be found!", "Game::Game");
		SNUFF_XASSERT(src_directory != nullptr && src_directory->IsString() == true, "The 'src_directory' CVar is corrupt or is not of a string type!", "Game::Game");

		path_ = src_directory->As<CVar::String>()->value();
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
		SNUFF_ASSERT_NOTNULL(render_device_, "Game::Verify::RenderDevice");

    js_init_.Set("Game", "Initialise");
    js_update_.Set("Game", "Update");
		js_fixed_update_.Set("Game", "FixedUpdate");
		js_draw_.Set("Game", "Draw");
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
		if (started_ == false)
		{
			return;
		}

		window_->ProcessMessages();
		keyboard_->Update();
		mouse_->Update();
	}

	//-------------------------------------------------------------------------------------------
	void Game::Update()
	{
		if (started_ == false)
		{
			return;
		}

		js_update_.Call(delta_time_);

		time_ += delta_time_;

		FixedUpdate();
	}

	//-------------------------------------------------------------------------------------------
	void Game::FixedUpdate()
	{
		if (started_ == false)
		{
			return;
		}

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
		if (started_ == false)
		{
			return;
		}

#ifdef SNUFF_BUILD_CONSOLE
		if (Console::Instance()->enabled())
		{
			qApp->processEvents();
		}
#endif
	}

	//-------------------------------------------------------------------------------------------
	void Game::SetTimePoint()
	{
		last_time_ = high_resolution_clock::now();
		current_time_ = high_resolution_clock::now();
	}

	//-------------------------------------------------------------------------------------------
	void Game::Run()
	{
		if (started_ == false)
		{
			return;
		}
		CalculateDeltaTime();
    UpdateConsole();
		UpdateInput();
		Update();
		Draw();
	}

	//-------------------------------------------------------------------------------------------
	void Game::CalculateDeltaTime()
	{
		current_time_ = high_resolution_clock::now();
		delta_time_ = duration_cast<duration<float, std::milli>>(current_time_ - last_time_).count() * 1e-3f;
		last_time_ = current_time_;
	}

	//-------------------------------------------------------------------------------------------
	void Game::Reload()
	{
		if (started_ == false)
		{
			return;
		}

		js_init_.Set("Game", "Initialise");
		js_update_.Set("Game", "Update");
		js_fixed_update_.Set("Game", "FixedUpdate");
		js_draw_.Set("Game", "Draw");
		js_shutdown_.Set("Game", "Shutdown");
		js_on_reload_.Set("Game", "OnReload");

		js_on_reload_.Call(FileWatch::Instance()->last_reloaded());
	}

	//-------------------------------------------------------------------------------------------
	void Game::Draw()
	{
    render_device_->StartDraw();
		js_draw_.Call(delta_time_);
    render_device_->Draw();
	}

	//-------------------------------------------------------------------------------------------
	void Game::Render(D3D11Camera* camera, D3D11RenderTarget* target)
	{
		if (started_ == false)
		{
			return;
		}

		if (camera == nullptr)
		{
			SNUFF_LOG_WARNING("No camera was set from rendering, rendering non-ui elements will fail");
    }
    D3D11RenderDevice::RenderCommand cmd;
    cmd.camera = camera;
    cmd.target = target;

    render_device_->ReceiveCommand(cmd);
	}

	//-------------------------------------------------------------------------------------------
	void Game::Notify(const Game::GameNotifications& evt)
	{
		switch (evt)
		{
		case Game::GameNotifications::kReload:
			Reload();
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
		js_shutdown_.Call();

		started_ = false;
	}

	//-------------------------------------------------------------------------------------------
	const std::string& Game::path() const
	{
		return path_;
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
	PlatformRenderDevice* Game::render_device()
	{
		return render_device_;
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
	const double& Game::delta_time() const
	{
		return delta_time_;
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
	void Game::set_render_device(PlatformRenderDevice* render_device)
	{
		SNUFF_ASSERT_NOTNULL(render_device, "Game::render_device");
		render_device_ = render_device;
	}

	//-------------------------------------------------------------------------------------------
	void Game::set_fixed_step(const double& step)
	{
		fixed_step_ = step;
	}

	//-------------------------------------------------------------------------------------------
	void Game::set_time(const double& time)
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
			{ "setFixedStep", JSSetFixedStep },
			{ "render", JSRender },
      { "cleanUp", JSCleanUp }
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

	//-------------------------------------------------------------------------------------------
	void Game::JSRender(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		wrapper.Check("OO");

    Game::Instance()->Render(wrapper.GetPointer<D3D11Camera>(0), wrapper.GetPointer<D3D11RenderTarget>(1));
	}

  //-------------------------------------------------------------------------------------------
  void Game::JSCleanUp(JS_ARGS args)
  {
    JSStateWrapper::Instance()->isolate()->LowMemoryNotification();
  }
}