#include "../input/keyboard.h"

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	Keyboard::Keyboard() :
		last_released_(Key::Keys::kNone),
		last_pressed_(Key::Keys::kNone)
	{
		ResetStates();
	}

	//-------------------------------------------------------------------------------------------
	void Keyboard::ResetStates()
	{
		for (unsigned int i = 0; i < 255; ++i)
		{
			KeyState& state = states_[i];
			state.pressed = false;
			state.released = false;
		}
	}

	//-------------------------------------------------------------------------------------------
	void Keyboard::Notify(KeyData data)
	{
		queue_.push(data);
	}

	//-------------------------------------------------------------------------------------------
	void Keyboard::Update()
	{
		ResetStates();

		while (queue_.empty() == false)
		{
			const KeyData& top = queue_.front();
			
			switch (top.evt)
			{
			case KeyEvent::kPressed:
				if (!states_[top.keycode].down)
				{
					states_[top.keycode].pressed = true;
				}

				states_[top.keycode].down = true;
				last_pressed_ = top.keycode;
				break;

			case KeyEvent::kReleased:
				states_[top.keycode].down = false;
				states_[top.keycode].released = true;
				last_released_ = top.keycode;
				break;
			}

			queue_.pop();
		}
	}

	//-------------------------------------------------------------------------------------------
	bool Keyboard::IsPressed(Key::Keys key)
	{
		return states_[key].pressed;
	}

	//-------------------------------------------------------------------------------------------
	bool Keyboard::IsDown(Key::Keys key)
	{
		return states_[key].down;
	}

	//-------------------------------------------------------------------------------------------
	bool Keyboard::IsReleased(Key::Keys key)
	{
		return states_[key].released;
	}

	//-------------------------------------------------------------------------------------------
	Keyboard::~Keyboard()
	{
		ResetStates();
	}
}