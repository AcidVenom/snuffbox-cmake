#include "../win32/win32_window.h"

#include "../application/logging.h"
#include "../application/game.h"

#include "../input/keyboard.h"
#include "../input/mouse.h"

#include "../platform/platform_render_device.h"
#include "../platform/platform_window.h"

#define SNUFF_WINDOW_CLASS "WIN32_WIN_CLASS"

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_CREATE)
		{
			SetWindowLongPtrA(hWnd, GWLP_USERDATA, (LONG_PTR)(((LPCREATESTRUCT)lParam)->lpCreateParams));
			return DefWindowProcA(hWnd, message, wParam, lParam);
		}

		Win32Window *window = reinterpret_cast<Win32Window*>(GetWindowLongPtrA(hWnd, GWLP_USERDATA));

		if (!window)
		{
			return DefWindowProcA(hWnd, message, wParam, lParam);
		}

		POINT p;
		if (GetCursorPos(&p))
		{
			ScreenToClient(hWnd, &p);
			window->SetMousePosition(p.x, p.y);
		}

		switch (message)
		{
		case WM_SIZE:
			window->OnResize(lParam);
			break;

		case WM_SETFOCUS:
			window->OnSetFocus();
			break;

		case WM_KILLFOCUS:
			window->OnKillFocus();
			break;

		case WM_CLOSE:
			window->SendQuitMessage();
			break;

		case WM_LBUTTONDBLCLK:
			window->OnMouseDbl(0, lParam, wParam);
			break;

		case WM_LBUTTONDOWN:
			window->OnMouseDown(0, lParam, wParam);
			break;

		case WM_LBUTTONUP:
			window->OnMouseUp(0, lParam, wParam);
			break;

		case WM_RBUTTONDBLCLK:
			window->OnMouseDbl(1, lParam, wParam);
			break;

		case WM_RBUTTONDOWN:
			window->OnMouseDown(1, lParam, wParam);
			break;

		case WM_RBUTTONUP:
			window->OnMouseUp(1, lParam, wParam);
			break;

		case WM_MBUTTONDBLCLK:
			window->OnMouseDbl(2, lParam, wParam);
			break;

		case WM_MBUTTONDOWN:
			window->OnMouseDown(2, lParam, wParam);
			break;

		case WM_MBUTTONUP:
			window->OnMouseUp(2, lParam, wParam);
			break;

		case WM_MOUSEMOVE:
			window->OnMouseMove(lParam, wParam);
			break;

		case WM_MOUSEWHEEL:
			window->OnMouseWheel(lParam, wParam);
			break;

		case WM_KEYDOWN:
			window->OnKeyDown(lParam, wParam);
			break;

		case WM_KEYUP:
			window->OnKeyUp(lParam, wParam);
			break;

    default:
      return DefWindowProcA(hWnd, message, wParam, lParam);
		}

		return 0;
	}

	//-------------------------------------------------------------------------------------------
	Win32Window::Win32Window(const int& x, const int& y, const int& w, const int& h, const std::string& name) :
		x_(x),
		y_(y),
		width_(w),
		height_(h),
		name_(name),
		focussed_(true)
	{
		Initialise();
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::Initialise()
	{
		WNDCLASSA wndClass;

		instance_ = GetModuleHandle(0);

		ZeroMemory(&wndClass, sizeof(WNDCLASS));

		wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wndClass.lpfnWndProc = WndProc;
		wndClass.hInstance = instance_;
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
		wndClass.lpszClassName = SNUFF_WINDOW_CLASS;
		wndClass.cbWndExtra = sizeof(void*);
		wndClass.cbClsExtra = sizeof(WNDCLASS);

		SNUFF_XASSERT(RegisterClassA(&wndClass), "Unable to register window class!", "Win32Window::Initialise");

		RECT clientSize;
		clientSize.left = clientSize.top = 0;
		clientSize.right = width_;
		clientSize.bottom = height_;

		int style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX;

		AdjustWindowRect(&clientSize, style, FALSE);
		unsigned int actualWidth = clientSize.right - clientSize.left;
		unsigned int actualHeight = clientSize.bottom - clientSize.top;

		x_ = x_ == SNUFF_WINDOW_CENTERED ? (GetSystemMetrics(SM_CXSCREEN) - actualWidth) / 2 : x_;
		y_ = y_ == SNUFF_WINDOW_CENTERED ? (GetSystemMetrics(SM_CYSCREEN) - actualHeight) / 2 : y_;

		handle_ = CreateWindowA(wndClass.lpszClassName, name_.c_str(),
			style, x_, y_, actualWidth, actualHeight, GetDesktopWindow(), NULL,
			wndClass.hInstance, this);
		
		SNUFF_XASSERT(handle_, "Unable to create window!", "Win32Window::Initialise");
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::Show()
	{
		ShowWindow(handle_, SW_SHOWNORMAL);
	}

  //-------------------------------------------------------------------------------------------
  void Win32Window::SetSize(const int& w, const int& h)
  {
    RECT client;
    client.left = client.top = 0;
    client.right = w;
    client.bottom = h;

    int style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX;

    AdjustWindowRect(&client, style, FALSE);
    unsigned int width = client.right - client.left;
    unsigned int height = client.bottom - client.top;

    width_ = width;
    height_ = height;

    SetWindowPos(handle_, HWND_TOP, (GetSystemMetrics(SM_CXSCREEN) - width) / 2, (GetSystemMetrics(SM_CYSCREEN) - height) / 2, width, height, NULL);

    D3D11RenderDevice::Instance()->ResizeBuffers(w, h);
  }

  //-------------------------------------------------------------------------------------------
  void Win32Window::SetName(const std::string& name)
  {
    SetWindowTextA(handle_, name.c_str());
    name_ = name;
  }

	//-------------------------------------------------------------------------------------------
	void Win32Window::ProcessMessages()
	{
		if (handle_ != nullptr)
		{
			HWND hwnd = static_cast<HWND>(handle_);
			MSG msg;
			while (PeekMessageA(&msg, hwnd, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessageA(&msg);
			}
		}
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::SendQuitMessage()
	{
		Game::Instance()->Notify(Game::GameNotifications::kQuit);
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::OnResize(const LPARAM& lParam)
	{
		width_ = LOWORD(lParam);
		height_ = HIWORD(lParam);

		PlatformRenderDevice::Instance()->ResizeBuffers(width_, height_);
		SNUFF_LOG_INFO("The window was resized to " + std::to_string(width_) + " x " + std::to_string(height_));
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::OnSetFocus()
	{
		focussed_ = true;
		SNUFF_LOG_INFO("The window regained focus");
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::OnKillFocus()
	{
		focussed_ = false;
		SNUFF_LOG_INFO("The window lost focus");
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::OnKeyDown(const LPARAM& lParam, const WPARAM& wParam)
	{
		Keyboard::KeyData data;
		data.evt = Keyboard::KeyEvent::kPressed;
		data.keycode = static_cast<Key::Keys>(wParam);

		Game::Instance()->keyboard()->Notify(data);
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::OnKeyUp(const LPARAM& lParam, const WPARAM& wParam)
	{
		Keyboard::KeyData data;
		data.evt = Keyboard::KeyEvent::kReleased;
		data.keycode = static_cast<Key::Keys>(wParam);

		Game::Instance()->keyboard()->Notify(data);
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::OnMouseDown(const int& button, const LPARAM& lParam, const WPARAM& wParam)
	{
		Mouse::MouseData data;

		data.x = mouse_position_.x;
		data.y = mouse_position_.y;
		data.button = static_cast<Mouse::MouseButton>(button);
		data.evt = Mouse::MouseEvent::kPressed;

		Game::Instance()->mouse()->Notify(data);
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::OnMouseUp(const int& button, const LPARAM& lParam, const WPARAM& wParam)
	{
		Mouse::MouseData data;

		data.x = mouse_position_.x;
		data.y = mouse_position_.y;
		data.button = static_cast<Mouse::MouseButton>(button);
		data.evt = Mouse::MouseEvent::kReleased;

		Game::Instance()->mouse()->Notify(data);
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::OnMouseDbl(const int& button, const LPARAM& lParam, const WPARAM& wParam)
	{
		Mouse::MouseData data;

		data.x = mouse_position_.x;
		data.y = mouse_position_.y;
		data.button = static_cast<Mouse::MouseButton>(button);
		data.evt = Mouse::MouseEvent::kDblClk;

		Game::Instance()->mouse()->Notify(data);
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::OnMouseMove(const LPARAM& lParam, const WPARAM& wParam)
	{
		Mouse::MouseData data;

		data.x = mouse_position_.x;
		data.y = mouse_position_.y;
		data.evt = Mouse::MouseEvent::kMove;

		Game::Instance()->mouse()->Notify(data);
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::OnMouseWheel(const LPARAM& lParam, const WPARAM& wParam)
	{
		Mouse::MouseData data;

		data.x = mouse_position_.x;
		data.y = mouse_position_.y;
		data.evt = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? Mouse::MouseEvent::kWheelUp : Mouse::MouseEvent::kWheelDown;

		Game::Instance()->mouse()->Notify(data);
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::SetMousePosition(const int& x, const int& y)
	{
		mouse_position_.x = x;
		mouse_position_.y = y;
	}

	//-------------------------------------------------------------------------------------------
	const int& Win32Window::x() const
	{
		return x_;
	}

	//-------------------------------------------------------------------------------------------
	const int& Win32Window::y() const
	{
		return y_;
	}

	//-------------------------------------------------------------------------------------------
	const int& Win32Window::width() const
	{
		return width_;
	}

	//-------------------------------------------------------------------------------------------
	const int& Win32Window::height() const
	{
		return height_;
	}

  //-------------------------------------------------------------------------------------------
  const std::string& Win32Window::name() const
  {
    return name_;
  }

	//-------------------------------------------------------------------------------------------
	HWND Win32Window::handle()
	{
		return handle_;
	}

	//-------------------------------------------------------------------------------------------
	const bool& Win32Window::focussed() const
	{
		return focussed_;
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::set_width(const int& w)
	{
		width_ = w;
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::set_height(const int& h)
	{
		height_ = h;
	}

	//-------------------------------------------------------------------------------------------
	Win32Window::~Win32Window()
	{
		SNUFF_ASSERT_NOTNULL(handle_, "Win32Window::~Win32Window");
		DestroyWindow(handle_);
		UnregisterClassA(SNUFF_WINDOW_CLASS, instance_);
		SNUFF_LOG_INFO("Destroyed the window with name '" + name_ + "'");
	}

  //-------------------------------------------------------------------------------------------
  void Win32Window::RegisterJS(JS_SINGLETON obj)
  {
    JSFunctionRegister funcs[] = {
      { "setSize", JSSetSize },
      { "size", JSSize },
      { "setName", JSSetName },
      { "name", JSName }
    };

    JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
  }

  //-------------------------------------------------------------------------------------------
  void Win32Window::JSSetSize(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    Window* self = Game::Instance()->window();

    if (wrapper.Check("NN") == true)
    {
      self->SetSize(wrapper.GetValue<int>(0, 640), wrapper.GetValue<int>(1, 480));
    }
  }

  //-------------------------------------------------------------------------------------------
  void Win32Window::JSSize(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    Window* self = Game::Instance()->window();

    v8::Handle<v8::Object> obj = JSWrapper::CreateObject();

    JSWrapper::SetObjectValue(obj, "w", self->width());
    JSWrapper::SetObjectValue(obj, "h", self->height());

    wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
  }

  //-------------------------------------------------------------------------------------------
  void Win32Window::JSName(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    Window* self = Game::Instance()->window();

    wrapper.ReturnValue<std::string>(self->name());
  }

  //-------------------------------------------------------------------------------------------
  void Win32Window::JSSetName(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    Window* self = Game::Instance()->window();

    if (wrapper.Check("S") == true)
    {
      self->SetName(wrapper.GetValue<std::string>(0, self->name()));
    }
  }
}