#include "../input/mouse.h"
#include "../input/mouse_area.h"

#include "../memory/allocated_memory.h"
#include "../memory/shared_ptr.h"

#include "../platform/platform_window.h"
#include "../d3d11/d3d11_render_device.h"
#include "../d3d11/d3d11_viewport.h"
#include "../d3d11/d3d11_render_settings.h"
#include "../application/game.h"

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	bool Mouse::mouse_available_ = false;

	//-------------------------------------------------------------------------------------------
	const bool& Mouse::mouse_available()
	{
		return mouse_available_;
	}

  //-------------------------------------------------------------------------------------------
  bool Mouse::MouseAreaSorter::operator()(MouseArea* a, MouseArea* b)
  {
    return a->GetZ() > b->GetZ();
  }

	//-------------------------------------------------------------------------------------------
	Mouse::Mouse() :
		x_(0),
		y_(0),
		dx_(0),
		dy_(0),
		prev_x_(0),
		prev_y_(0)
	{
		mouse_available_ = true;
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
      case MouseEvent::kMove:
        x_ = top.x;
        y_ = top.y;
        break;

			case MouseEvent::kPressed:
				states_[top.button].down = true;
        states_[top.button].pressed = true;
        x_ = top.x;
        y_ = top.y;
				break;

			case MouseEvent::kReleased:
				states_[top.button].down = false;
				states_[top.button].released = true;
				break;

			case MouseEvent::kDblClk:
				states_[top.button].down = true;
				states_[top.button].double_clicked = true;
        x_ = top.x;
        y_ = top.y;
				break;

			case MouseEvent::kWheelDown:
				wheel_down_ = true;
				break;

			case MouseEvent::kWheelUp:
				wheel_up_ = true;
				break;
			}

			queue_.pop();
		}

		dx_ = x_ - prev_x_;
		dy_ = y_ - prev_y_;

		prev_x_ = x_;
		prev_y_ = y_;

    std::sort(mouse_areas_.begin(), mouse_areas_.end(), MouseAreaSorter());

    bool do_callback = true;
		bool callback = true;
    for (unsigned int i = 0; i < mouse_areas_.size(); ++i)
    {
      if (mouse_areas_.at(i)->activated() == false)
      {
        continue;
      }
      do_callback = !mouse_areas_.at(i)->Check(this, callback);

			if (do_callback == false)
			{
				callback = false;
			}
    }
	}

	//-------------------------------------------------------------------------------------------
	const bool& Mouse::IsPressed(const Mouse::MouseButton& button) const
	{
		return states_[button].pressed;
	}

	//-------------------------------------------------------------------------------------------
	const bool& Mouse::IsDown(const Mouse::MouseButton& button) const
	{
		return states_[button].down;
	}

	//-------------------------------------------------------------------------------------------
	const bool& Mouse::IsReleased(const Mouse::MouseButton& button) const
	{
		return states_[button].released;
	}

	//-------------------------------------------------------------------------------------------
	const bool& Mouse::IsDoubleClicked(const Mouse::MouseButton& button) const
	{
		return states_[button].double_clicked;
	}

	//-------------------------------------------------------------------------------------------
	Mouse::float2 Mouse::Position(const Mouse::MousePosition& type)
	{
		float2 p;
		float px = static_cast<float>(x());
		float py = static_cast<float>(y());

		if (type == MousePosition::kAbsolute)
		{
			p.x = px;
			p.y = py;
			return p;
		}

		Window* window = Game::Instance()->window();

		float w = static_cast<float>(window->width());
		float h = static_cast<float>(window->height());

		D3D11Viewport* vp = D3D11RenderDevice::Instance()->viewport();
		
		float x1 = vp->x();
		float y1 = vp->y();
		float x2 = vp->width();
		float y2 = vp->height();

		float xx = (px - x1) / x2;
		float yy = (py - y1) / y2;

		xx = xx * 2 - 1;
		yy = yy * 2 - 1;

		if (xx < -1)
		{
			xx = -1;
		}

		if (yy < -1)
		{
			yy = -1;
		}

		if (xx > 1)
		{
			xx = 1;
		}

		if (yy > 1)
		{
			yy = 1;
		}

		if (type == MousePosition::kScreen)
		{
			p.x = xx;
			p.y = yy;

			return p;
		}
		else if (type == MousePosition::kRelative)
		{
			const XMFLOAT2& res = D3D11RenderSettings::Instance()->resolution();
			p.x = xx * res.x / 2.0f;
			p.y = yy * res.y / 2.0f;
			return p;
		}

		p.x = 0.0f;
		p.y = 0.0f;

		return p;
	}

  //-------------------------------------------------------------------------------------------
  void Mouse::AddMouseArea(MouseArea* area)
  {
		for (MouseArea* it : mouse_areas_)
		{
			if (it == area)
			{
				return;
			}
		}
    mouse_areas_.push_back(area);
  }

  //-------------------------------------------------------------------------------------------
  void Mouse::RemoveMouseArea(MouseArea* area)
  {
    MouseArea* it = nullptr;

    for (unsigned int i = 0; i < mouse_areas_.size(); ++i)
    {
      it = mouse_areas_.at(i);

      if (it == area)
      {
        mouse_areas_.erase(mouse_areas_.begin() + i);
				break;
      }
    }
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
		mouse_available_ = false;
		ResetStates();
	}

	//-------------------------------------------------------------------------------------------
	void Mouse::JSEnumerate()
	{
		v8::Handle<v8::Object> obj = JSWrapper::CreateObject();

		JSWrapper::SetObjectValue<double>(obj, "Left", Mouse::MouseButton::kLeft);
		JSWrapper::SetObjectValue<double>(obj, "Right", Mouse::MouseButton::kRight);
		JSWrapper::SetObjectValue<double>(obj, "Middle", Mouse::MouseButton::kMiddle);

		JSStateWrapper::Instance()->RegisterGlobal("MouseButton", obj);

		obj = JSWrapper::CreateObject();

		JSWrapper::SetObjectValue<double>(obj, "Absolute", Mouse::MousePosition::kAbsolute);
		JSWrapper::SetObjectValue<double>(obj, "Screen", Mouse::MousePosition::kScreen);
		JSWrapper::SetObjectValue<double>(obj, "Relative", Mouse::MousePosition::kRelative);

		JSStateWrapper::Instance()->RegisterGlobal("MousePosition", obj);
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

		JSEnumerate();

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

		if (wrapper.Check("N") == true)
		{
			Mouse::float2 p = self->Position(static_cast<Mouse::MousePosition>(wrapper.GetValue<int>(0, 0)));

			v8::Handle<v8::Object> obj = wrapper.CreateObject();
			JSWrapper::SetObjectValue(obj, "x", static_cast<double>(p.x));
			JSWrapper::SetObjectValue(obj, "y", static_cast<double>(p.y));

			wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
		}
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