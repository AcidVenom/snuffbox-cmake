#include "../input/mouse.h"

#include "../memory/allocated_memory.h"
#include "../memory/shared_ptr.h"

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	Mouse::Mouse() :
		x_(0),
		y_(0),
		dx_(0),
		dy_(0),
		prev_x_(0),
		prev_y_(0)
	{
		ResetStates();
	}

  //-------------------------------------------------------------------------------------------
  Mouse* Mouse::Instance()
  {
    static SharedPtr<Mouse> mouse = AllocatedMemory::Instance().Construct<Mouse>();
    return mouse.get();
  }

	//-------------------------------------------------------------------------------------------
	void Mouse::ResetStates()
	{
		for (unsigned int i = 0; i < 4; ++i)
		{
			ButtonState& state = states_[i];
			state.pressed = false;
			state.released = false;
			state.double_clicked = false;
		}

		wheel_up_ = false;
		wheel_down_ = false;
	}

	//-------------------------------------------------------------------------------------------
	void Mouse::Notify(Mouse::MouseData data)
	{
		queue_.push(data);
	}

	//-------------------------------------------------------------------------------------------
	void Mouse::Update()
	{
		ResetStates();

		dx_ = 0;
		dy_ = 0;

		while (queue_.empty() == false)
		{
			const MouseData& top = queue_.front();

			switch (top.evt)
			{
			case MouseEvent::kPressed:
				if (!states_[top.button].down)
				{
					states_[top.button].pressed = true;
				}

				states_[top.button].down = true;
				break;

			case MouseEvent::kReleased:
				states_[top.button].down = false;
				states_[top.button].released = true;
				break;

			case MouseEvent::kDblClk:
				states_[top.button].down = true;
				states_[top.button].double_clicked = true;
				break;

			case MouseEvent::kWheelDown:
				wheel_down_ = true;
				break;

			case MouseEvent::kWheelUp:
				wheel_up_ = true;
				break;

			case MouseEvent::kMove:
				break;
			}

			x_ = top.x;
			y_ = top.y;

			queue_.pop();
		}

		dx_ = x_ - prev_x_;
		dy_ = y_ - prev_y_;

		prev_x_ = x_;
		prev_y_ = y_;
	}

	//-------------------------------------------------------------------------------------------
	const bool& Mouse::IsPressed(Mouse::MouseButton button) const
	{
		return states_[button].pressed;
	}

	//-------------------------------------------------------------------------------------------
	const bool& Mouse::IsDown(Mouse::MouseButton button) const
	{
		return states_[button].down;
	}

	//-------------------------------------------------------------------------------------------
	const bool& Mouse::IsReleased(Mouse::MouseButton button) const
	{
		return states_[button].released;
	}

	//-------------------------------------------------------------------------------------------
	const bool& Mouse::IsDoubleClicked(Mouse::MouseButton button) const
	{
		return states_[button].double_clicked;
	}

	//-------------------------------------------------------------------------------------------
	const bool& Mouse::wheel_down() const
	{
		return wheel_down_;
	}

	//-------------------------------------------------------------------------------------------
	const bool& Mouse::wheel_up() const
	{
		return wheel_up_;
	}

	//-------------------------------------------------------------------------------------------
	const int& Mouse::x() const
	{
		return x_;
	}

	//-------------------------------------------------------------------------------------------
	const int& Mouse::y() const
	{
		return y_;
	}

	//-------------------------------------------------------------------------------------------
	const int& Mouse::dx() const
	{
		return dx_;
	}

	//-------------------------------------------------------------------------------------------
	const int& Mouse::dy() const
	{
		return dy_;
	}

	//-------------------------------------------------------------------------------------------
	Mouse::~Mouse()
	{
		ResetStates();
	}

	//-------------------------------------------------------------------------------------------
	void Mouse::JSEnumerateButtons()
	{
		v8::Handle<v8::Object> obj = JSWrapper::CreateObject();

		JSWrapper::SetObjectValue<double>(obj, "Left", Mouse::MouseButton::kLeft);
		JSWrapper::SetObjectValue<double>(obj, "Right", Mouse::MouseButton::kRight);
		JSWrapper::SetObjectValue<double>(obj, "Middle", Mouse::MouseButton::kMiddle);

		JSStateWrapper::Instance()->RegisterGlobal("MouseButton", obj);
	}

	//-------------------------------------------------------------------------------------------
	void Mouse::RegisterJS(JS_SINGLETON obj)
	{
		JSFunctionRegister funcs[] = {
			{ "isDown", JSIsDown },
			{ "isPressed", JSIsPressed },
			{ "isReleased", JSIsReleased },
			{ "isDoubleClicked", JSIsDoubleClicked },
			{ "wheelDown", JSWheelDown },
			{ "wheelUp", JSWheelUp },
			{ "position", JSPosition },
			{ "movement", JSMovement }
		};

		JSEnumerateButtons();

		JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
	}

	//-------------------------------------------------------------------------------------------
	void Mouse::JSIsDown(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		Mouse* self = Mouse::Instance();
		wrapper.Check("N");

		wrapper.ReturnValue<bool>(self->IsDown(static_cast<MouseButton>(wrapper.GetValue<int>(0, 0))));
	}

	//-------------------------------------------------------------------------------------------
	void Mouse::JSIsPressed(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		Mouse* self = Mouse::Instance();
		wrapper.Check("N");

		wrapper.ReturnValue<bool>(self->IsPressed(static_cast<MouseButton>(wrapper.GetValue<int>(0, 0))));
	}

	//-------------------------------------------------------------------------------------------
	void Mouse::JSIsReleased(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		Mouse* self = Mouse::Instance();
		wrapper.Check("N");

		wrapper.ReturnValue<bool>(self->IsReleased(static_cast<MouseButton>(wrapper.GetValue<int>(0, 0))));
	}

	//-------------------------------------------------------------------------------------------
	void Mouse::JSIsDoubleClicked(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		Mouse* self = Mouse::Instance();
		wrapper.Check("N");

		wrapper.ReturnValue<bool>(self->IsDoubleClicked(static_cast<MouseButton>(wrapper.GetValue<int>(0, 0))));
	}

	//-------------------------------------------------------------------------------------------
	void Mouse::JSWheelDown(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		Mouse* self = Mouse::Instance();

		wrapper.ReturnValue<bool>(self->wheel_down());
	}

	//-------------------------------------------------------------------------------------------
	void Mouse::JSWheelUp(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		Mouse* self = Mouse::Instance();

		wrapper.ReturnValue<bool>(self->wheel_up());
	}

	//-------------------------------------------------------------------------------------------
	void Mouse::JSPosition(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		Mouse* self = Mouse::Instance();

		int x = self->x();
		int y = self->y();

		v8::Handle<v8::Object> obj = wrapper.CreateObject();
		JSWrapper::SetObjectValue(obj, "x", static_cast<double>(x));
		JSWrapper::SetObjectValue(obj, "y", static_cast<double>(y));

		wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
	}

	//-------------------------------------------------------------------------------------------
	void Mouse::JSMovement(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		Mouse* self = Mouse::Instance();

		int dx = self->dx();
		int dy = self->dy();

		v8::Handle<v8::Object> obj = wrapper.CreateObject();
		JSWrapper::SetObjectValue(obj, "x", static_cast<double>(dx));
		JSWrapper::SetObjectValue(obj, "y", static_cast<double>(dy));

		wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
	}
}