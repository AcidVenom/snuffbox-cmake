#include "../win32/win32_window.h"

#include "../application/logging.h"
#include "../application/game.h"

#include "../input/keyboard.h"
#include "../input/mouse.h"

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

		if (message == WM_SIZE)
		{
			window->set_width(LOWORD(lParam));
			window->set_height(HIWORD(lParam));
		}

		POINT p;
		if (GetCursorPos(&p))
		{
			ScreenToClient(hWnd, &p);
			window->SetMousePosition(p.x, p.y);
		}

		switch (message)
		{
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
		}
		return DefWindowProcA(hWnd, message, wParam, lParam);
	}

	//-------------------------------------------------------------------------------------------
	Win32Window::Win32Window(int x, int y, int w, int h, std::string name) :
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
	void Win32Window::OnKeyDown(LPARAM lParam, WPARAM wParam)
	{
		Keyboard::KeyData data;
		data.evt = Keyboard::KeyEvent::kPressed;
		data.keycode = static_cast<Key::Keys>(wParam);

		Game::Instance()->keyboard()->Notify(data);
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::OnKeyUp(LPARAM lParam, WPARAM wParam)
	{
		Keyboard::KeyData data;
		data.evt = Keyboard::KeyEvent::kReleased;
		data.keycode = static_cast<Key::Keys>(wParam);

		Game::Instance()->keyboard()->Notify(data);
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::OnMouseDown(int button, LPARAM lParam, WPARAM wParam)
	{
		Mouse::MouseData data;

		data.x = mouse_position_.x;
		data.y = mouse_position_.y;
		data.button = static_cast<Mouse::MouseButton>(button);
		data.evt = Mouse::MouseEvent::kPressed;

		Game::Instance()->mouse()->Notify(data);
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::OnMouseUp(int button, LPARAM lParam, WPARAM wParam)
	{
		Mouse::MouseData data;

		data.x = mouse_position_.x;
		data.y = mouse_position_.y;
		data.button = static_cast<Mouse::MouseButton>(button);
		data.evt = Mouse::MouseEvent::kReleased;

		Game::Instance()->mouse()->Notify(data);
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::OnMouseDbl(int button, LPARAM lParam, WPARAM wParam)
	{
		Mouse::MouseData data;

		data.x = mouse_position_.x;
		data.y = mouse_position_.y;
		data.button = static_cast<Mouse::MouseButton>(button);
		data.evt = Mouse::MouseEvent::kDblClk;

		Game::Instance()->mouse()->Notify(data);
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::OnMouseMove(LPARAM lParam, WPARAM wParam)
	{
		Mouse::MouseData data;

		data.x = mouse_position_.x;
		data.y = mouse_position_.y;
		data.evt = Mouse::MouseEvent::kMove;

		Game::Instance()->mouse()->Notify(data);
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::OnMouseWheel(LPARAM lParam, WPARAM wParam)
	{
		Mouse::MouseData data;

		data.x = mouse_position_.x;
		data.y = mouse_position_.y;
		data.evt = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? Mouse::MouseEvent::kWheelUp : Mouse::MouseEvent::kWheelDown;

		Game::Instance()->mouse()->Notify(data);
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::SetMousePosition(int x, int y)
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
	const void* Win32Window::handle() const
	{
		return handle_;
	}

	//-------------------------------------------------------------------------------------------
	const bool& Win32Window::focussed() const
	{
		return focussed_;
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::set_width(int w)
	{
		width_ = w;
	}

	//-------------------------------------------------------------------------------------------
	void Win32Window::set_height(int h)
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
}