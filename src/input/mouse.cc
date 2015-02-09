#include "../input/mouse.h"

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

			dx_ = x_ - prev_x_;
			dy_ = y_ - prev_y_;

			prev_x_ = x_;
			prev_y_ = y_;

			queue_.pop();
		}
	}

	//-------------------------------------------------------------------------------------------
	bool Mouse::IsPressed(Mouse::MouseButton button)
	{
		return states_[button].pressed;
	}

	//-------------------------------------------------------------------------------------------
	bool Mouse::IsDown(Mouse::MouseButton button)
	{
		return states_[button].down;
	}

	//-------------------------------------------------------------------------------------------
	bool Mouse::IsReleased(Mouse::MouseButton button)
	{
		return states_[button].released;
	}

	//-------------------------------------------------------------------------------------------
	bool Mouse::IsDoubleClicked(Mouse::MouseButton button)
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
}