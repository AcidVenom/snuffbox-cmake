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
      { "isReleased", JSIsReleased }
    };

    JSEnumerateKeys();

    JSFunctionRegister::Register(funcs, 3, obj);
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
}