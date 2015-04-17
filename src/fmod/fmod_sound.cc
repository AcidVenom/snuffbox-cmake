#include "../fmod/fmod_sound.h"
#include "../content/content_manager.h"

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
  Sound::Sound() :
    sound_(nullptr),
    Content(ContentTypes::kSound)
  {

  }

  //-------------------------------------------------------------------------------------------
  void Sound::Load(const std::string& path)
  {
    if (sound_ != nullptr)
    {
      sound_->release();
    }
    sound_ = SoundSystem::Instance()->Load(path);
  }

  //-------------------------------------------------------------------------------------------
  FMOD::Sound* Sound::get()
  {
    return sound_;
  }

  //-------------------------------------------------------------------------------------------
  Sound::~Sound()
  {
    if (sound_ != nullptr && is_valid() == true && SoundSystem::IsAvailable())
    {
      sound_->release();
    }
  }
}