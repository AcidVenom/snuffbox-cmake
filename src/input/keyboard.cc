#include "../input/keyboard.h"

#include "../memory/allocated_memory.h"
#include "../memory/shared_ptr.h"

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
  Keyboard* Keyboard::Instance()
  {
    static SharedPtr<Keyboard> keyboard = AllocatedMemory::Instance().Construct<Keyboard>();
    return keyboard.get();
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
	void Keyboard::Notify(const KeyData& data)
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
	const bool& Keyboard::IsPressed(const Key::Keys& key) const
	{
		return states_[key].pressed;
	}

	//-------------------------------------------------------------------------------------------
	const bool& Keyboard::IsDown(const Key::Keys& key) const
	{
		return states_[key].down;
	}

	//-------------------------------------------------------------------------------------------
	const bool& Keyboard::IsReleased(const Key::Keys& key) const
	{
		return states_[key].released;
	}

	//-------------------------------------------------------------------------------------------
	const Key::Keys& Keyboard::last_pressed() const
	{
		return last_pressed_;
	}

	//-------------------------------------------------------------------------------------------
	const Key::Keys& Keyboard::last_released() const
	{
		return last_released_;
	}

	//-------------------------------------------------------------------------------------------
	Keyboard::~Keyboard()
	{
		ResetStates();
	}

  //-------------------------------------------------------------------------------------------
  void Keyboard::JSEnumerateKeys()
  {
    v8::Handle<v8::Object> obj = JSWrapper::CreateObject();
    for (int i = 0; i < 255; ++i)
    {
      JSWrapper::SetObjectValue<double>(obj, Key::KeyToString(static_cast<Key::Keys>(i)), static_cast<double>(i));
    }

    JSStateWrapper::Instance()->RegisterGlobal("Key", obj);
  }

  //-------------------------------------------------------------------------------------------
  void Keyboard::RegisterJS(JS_SINGLETON obj)
  {
    JSFunctionRegister funcs[] = {
      { "isPressed", JSIsPressed },
      { "isDown", JSIsDown },
      { "isReleased", JSIsReleased },
			{ "lastPressed", JSLastPressed },
			{ "lastReleased", JSLastReleased }
    };

    JSEnumerateKeys();

		JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
  }

  //-------------------------------------------------------------------------------------------
  void Keyboard::JSIsPressed(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    wrapper.Check("N");

    wrapper.ReturnValue<bool>(Keyboard::Instance()->IsPressed(static_cast<Key::Keys>(wrapper.GetValue<int>(0, 0))));
  }

  //-------------------------------------------------------------------------------------------
  void Keyboard::JSIsDown(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    wrapper.Check("N");

    wrapper.ReturnValue<bool>(Keyboard::Instance()->IsDown(static_cast<Key::Keys>(wrapper.GetValue<int>(0, 0))));
  }

  //-------------------------------------------------------------------------------------------
  void Keyboard::JSIsReleased(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    wrapper.Check("N");

    wrapper.ReturnValue<bool>(Keyboard::Instance()->IsReleased(static_cast<Key::Keys>(wrapper.GetValue<int>(0, 0))));
  }

	//-------------------------------------------------------------------------------------------
	void Keyboard::JSLastPressed(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		wrapper.ReturnValue<std::string>(Key::KeyToString(Keyboard::Instance()->last_pressed()));
	}

	//-------------------------------------------------------------------------------------------
	void Keyboard::JSLastReleased(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		wrapper.ReturnValue<std::string>(Key::KeyToString(Keyboard::Instance()->last_released()));
	}
}