#include "../animation/sprite_animation.h"
#include "../animation/anim.h"
#include "../content/content_manager.h"
#include "../d3d11/elements/d3d11_render_element.h"
#include "../d3d11/d3d11_texture.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	SpriteAnimation::SpriteAnimation() :
		parent_(nullptr),
		anim_(nullptr),
		current_frame_(0),
		speed_(15.0f),
		elapsed_time_(0.0f),
		is_playing_(false),
		texture_(nullptr)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	SpriteAnimation::SpriteAnimation(JS_ARGS args) : 
		parent_(nullptr),
		anim_(nullptr),
		current_frame_(0),
		speed_(15.0f),
		elapsed_time_(0.0f),
		is_playing_(false),
		texture_(nullptr)
	{
		JSWrapper wrapper(args);

		if (wrapper.Check("SS") == true)
		{
			anim_ = ContentManager::Instance()->Get<Anim>(wrapper.GetValue<std::string>(0, "undefined"));
			texture_ = ContentManager::Instance()->Get<D3D11Texture>(wrapper.GetValue<std::string>(1, "undefined"));
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void SpriteAnimation::Animate(const float& dt)
	{
		if (speed_ < 0.0f)
		{
			speed_ = 0.0f;
		}

		if (is_playing_ == false)
		{
			return;
		}

		if (texture_ != nullptr)
		{
			if (texture_->is_valid() == false)
			{
				texture_ = nullptr;
				return;
			}
		}
		else
		{
			return;
		}

		if (anim_ != nullptr)
		{
			if (anim_->is_valid() == false)
			{
				anim_ = nullptr;
				return;
			}
		}
		else
		{
			return;
		}

		float time = 0.0f;
		float total_time = 0.0f;

		current_frame_ = -1;

		Frame* frame = nullptr;
		for (int i = 0; i < anim_->NumFrames(); ++i)
		{
			time = CalculateFrameTime(i);
			total_time += (1.0f / speed_);
			frame = anim_->GetFrame(i);

			if (frame != nullptr)
			{
				total_time += frame->wait * (1.0f / speed_);
			}

			if (time <= elapsed_time_)
			{
				++current_frame_;
			}
		}

		parent_->set_animation_coordinates(anim_->GetFrame(current_frame_), texture_);

		elapsed_time_ += dt * speed_;

		while (elapsed_time_ >= total_time)
		{
			elapsed_time_ = elapsed_time_ - total_time;
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void SpriteAnimation::Play()
	{
		is_playing_ = true;
	}

	//---------------------------------------------------------------------------------------------------------
	void SpriteAnimation::Pause()
	{
		is_playing_ = false;
	}

	//---------------------------------------------------------------------------------------------------------
	void SpriteAnimation::Stop()
	{
		is_playing_ = false;
		elapsed_time_ = 0.0f;
		
		if (parent_ != nullptr && anim_ != nullptr && texture_ != nullptr)
		{
			parent_->set_animation_coordinates(anim_->GetFrame(0), texture_);
		}
	}

	//---------------------------------------------------------------------------------------------------------
	const bool& SpriteAnimation::IsPlaying()
	{
		return is_playing_;
	}

	//---------------------------------------------------------------------------------------------------------
	float SpriteAnimation::CalculateFrameTime(int i)
	{
		if (anim_->NumFrames() == 0)
		{
			return 0.0f;
		}

		float total = 0.0f;
		float speed = 1.0f / speed_;

		for (int i = 0; i < current_frame_ + 1; ++i)
		{
			Frame* frame = anim_->GetFrame(i);

			if (frame == nullptr)
			{
				continue;
			}

			total += speed;
			total += frame->wait * speed;
		}

		return total;
	}

	//---------------------------------------------------------------------------------------------------------
	const int& SpriteAnimation::current_frame() const
	{
		return current_frame_;
	}

	//---------------------------------------------------------------------------------------------------------
	const float& SpriteAnimation::speed() const
	{
		return speed_;
	}

	//---------------------------------------------------------------------------------------------------------
	void SpriteAnimation::set_current_frame(const int& frame)
	{
		if (anim_ == nullptr)
		{
			return;
		}

		int new_frame = frame;

		if (new_frame >= anim_->NumFrames() || new_frame < 0)
		{
			SNUFF_LOG_WARNING("Frame index '" + std::to_string(frame) + "' is out of bounds, frames size is " + std::to_string(anim_->NumFrames()));
			new_frame = 0;
		}

		float total_time = 0.0f;
		float time = 0.0f;

		Frame* current_frame = nullptr;
		for (int i = 0; i < frame; ++i)
		{
			time = CalculateFrameTime(i);
			total_time += (1.0f / speed_);
			
			current_frame = anim_->GetFrame(i);

			if (current_frame != nullptr)
			{
				total_time += current_frame->wait * (1.0f / speed_);
			}
		}

		elapsed_time_ = total_time;
	}

	//---------------------------------------------------------------------------------------------------------
	void SpriteAnimation::set_speed(const float& speed)
	{
		speed_ = speed;
	}

	//---------------------------------------------------------------------------------------------------------
	void SpriteAnimation::set_parent(D3D11RenderElement* element)
	{
		parent_ = element;
	}

	//---------------------------------------------------------------------------------------------------------
	SpriteAnimation::~SpriteAnimation()
	{
		if (parent_ != nullptr)
		{
			parent_->set_animation(nullptr);
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void SpriteAnimation::RegisterJS(JS_CONSTRUCTABLE obj)
	{
		JSFunctionRegister funcs[] = {
			{ "play", JSPlay },
			{ "pause", JSPause },
			{ "stop", JSStop },
			{ "isPlaying", JSIsPlaying },
			{ "setFrame", JSSetFrame },
			{ "currentFrame", JSCurrentFrame },
			{ "setSpeed", JSSetSpeed },
			{ "speed", JSSpeed }
		};

		JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
	}

	//---------------------------------------------------------------------------------------------------------
	void SpriteAnimation::JSPlay(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		wrapper.GetPointer<SpriteAnimation>(args.This())->Play();
	}

	//---------------------------------------------------------------------------------------------------------
	void SpriteAnimation::JSPause(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		wrapper.GetPointer<SpriteAnimation>(args.This())->Pause();
	}

	//---------------------------------------------------------------------------------------------------------
	void SpriteAnimation::JSStop(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		wrapper.GetPointer<SpriteAnimation>(args.This())->Stop();
	}

	//---------------------------------------------------------------------------------------------------------
	void SpriteAnimation::JSIsPlaying(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		wrapper.ReturnValue<bool>(wrapper.GetPointer<SpriteAnimation>(args.This())->IsPlaying());
	}

	//---------------------------------------------------------------------------------------------------------
	void SpriteAnimation::JSSetFrame(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		SpriteAnimation* self = wrapper.GetPointer<SpriteAnimation>(args.This());

		if (wrapper.Check("N") == true)
		{
			self->set_current_frame(wrapper.GetValue<int>(0, 0));
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void SpriteAnimation::JSCurrentFrame(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		wrapper.ReturnValue<double>(wrapper.GetPointer<SpriteAnimation>(args.This())->current_frame());
	}

	//---------------------------------------------------------------------------------------------------------
	void SpriteAnimation::JSSetSpeed(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		SpriteAnimation* self = wrapper.GetPointer<SpriteAnimation>(args.This());

		if (wrapper.Check("N") == true)
		{
			self->set_speed(wrapper.GetValue<float>(0, 0));
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void SpriteAnimation::JSSpeed(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		wrapper.ReturnValue<double>(wrapper.GetPointer<SpriteAnimation>(args.This())->speed());
	}
}