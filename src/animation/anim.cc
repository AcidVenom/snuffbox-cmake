#include "../animation/anim.h"
#include "../content/content_manager.h"
#include "../application/game.h"
#include "../platform/platform_text_file.h"

using namespace v8;

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	Anim::Anim() :
		Content(ContentTypes::kAnim),
		valid_(false)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	void Anim::Load(const std::string& path)
	{
		std::vector<SpriteAnimation::Frame> new_frames;
		std::string full_path = Game::Instance()->path() + "/" + path;

		TextFile file;

		if (file.Open(full_path) == true)
		{
			std::string stringified = file.Read();
			JSStateWrapper* wrapper = JSStateWrapper::Instance();
			Isolate* isolate = wrapper->isolate();

			HandleScope scope(isolate);

			TryCatch try_catch;
			Handle<Value> json = JSON::Parse(String::NewFromUtf8(isolate, stringified.c_str()));

			if (json.IsEmpty() == true)
			{
				std::string error;
				bool has_error = wrapper->GetException(&try_catch, &error);

				if (has_error == true)
				{
					SNUFF_LOG_ERROR(error);
				}

				return;
			}

			if (json->IsArray() == false)
			{
				SNUFF_LOG_ERROR("Anim file '" + path + "' is not of an array type");
				return;
			}

			Handle<Object> frames = json->ToObject();
			Handle<Value> frame_value;
			Handle<Object> frame;
			for (unsigned int i = 0; i < frames->GetPropertyNames()->Length(); ++i)
			{
				frame_value = frames->Get(i);

				if (frame_value->IsObject() == false)
				{
					SNUFF_LOG_ERROR("Frame '" + std::to_string(i) + "' of anim '" + path + "' is not of an object type");
					continue;
				}

				frame = frame_value->ToObject();

				SpriteAnimation::Frame new_frame;

				FillFrame(frame, &new_frame, i, path);

				new_frames.push_back(new_frame);
			}
		}
		else
		{
			SNUFF_LOG_ERROR("Could not open anim file '" + path + "'");
			return;
		}

		SNUFF_LOG_INFO("Frames: " + std::to_string(new_frames.size()));
		frames_ = new_frames;
		valid_ = true;
	}

	//---------------------------------------------------------------------------------------------------------
	void Anim::FillFrame(const Handle<Object>& obj, SpriteAnimation::Frame* frame, const int& i, const std::string& path)
	{
		frame->x = 0;
		frame->y = 0;
		frame->h = 0;
		frame->w = 0;
		frame->wait = 0.0f;

		Isolate* isolate = JSStateWrapper::Instance()->isolate();

		auto get_value = [obj, isolate, i , path](const std::string& field)
		{
			Handle<Value> value = obj->Get(String::NewFromUtf8(isolate, field.c_str()));
			if (value.IsEmpty() || value->IsNumber() == false)
			{
				SNUFF_LOG_ERROR("'" + field + "' of frame " + std::to_string(i) + " of anim '" + path + "' is undefined or not a number, defaulting to 0.0");
				return 0.0f;
			}

			return static_cast<float>(value->ToNumber()->Value());
		};

		frame->x = static_cast<int>(get_value("x"));
		frame->y = static_cast<int>(get_value("y"));
		frame->w = static_cast<int>(get_value("width"));
		frame->h = static_cast<int>(get_value("height"));
		frame->wait = get_value("wait");
	}

	//---------------------------------------------------------------------------------------------------------
	SpriteAnimation::Frame* Anim::GetFrame(const int& frame)
	{
		if (frame >= frames_.size() || frame < 0)
		{
			return nullptr;
		}

		return &frames_.at(frame);
	}

	//---------------------------------------------------------------------------------------------------------
	int Anim::NumFrames()
	{
		return static_cast<int>(frames_.size());
	}

	//---------------------------------------------------------------------------------------------------------
	Anim::~Anim()
	{

	}
}