#include "../fmod/fmod_sound_system.h"
#include "../fmod/fmod_sound.h"
#include "../content/content_manager.h"
#include "../application/logging.h"
#include "../application/game.h"

namespace snuffbox
{
  static bool is_available_ = false;

  //-------------------------------------------------------------------------------------------
  const bool& SoundSystem::IsAvailable()
  {
    return is_available_;
  }

  //-------------------------------------------------------------------------------------------
  SoundSystem::SoundSystem() :
    fmod_system_(nullptr)
  {
    FMOD_RESULT result;

    result = FMOD::System_Create(&fmod_system_);

    SNUFF_XASSERT(result == FMOD_OK, "Failed creating the FMOD sound system", "SoundSystem::SoundSystem");

    result = fmod_system_->init(512, FMOD_INIT_NORMAL, 0);

    SNUFF_XASSERT(result == FMOD_OK, "Failed initializing the FMOD sound system", "SoundSystem::SoundSystem");

    unsigned int version = 0;
    result = fmod_system_->getVersion(&version);
    SNUFF_XASSERT(result == FMOD_OK, "Failed retrieving the FMOD version", "SoundSystem::SoundSystem");

    SNUFF_LOG_SUCCESS("Succesfully initialized FMOD v" + std::to_string(version));

    is_available_ = true;
  }

  //-------------------------------------------------------------------------------------------
  SoundSystem* SoundSystem::Instance()
  {
    static SharedPtr<SoundSystem> sound_system = AllocatedMemory::Instance().Construct<SoundSystem>();
    return sound_system.get();
  }

  //-------------------------------------------------------------------------------------------
  void SoundSystem::AddChannelGroup(const std::string& channel)
  {
    if (channel_groups_.find(channel) != channel_groups_.end())
    {
      SNUFF_LOG_ERROR("Channel with name '" + channel + "' already exists");
      return;
    }

    FMOD_RESULT result;

    FMOD::ChannelGroup* channel_group;
    result = fmod_system_->createChannelGroup(channel.c_str(), &channel_group);

    channel_groups_.emplace(channel, channel_group);
    SNUFF_LOG_INFO("Added channel group '" + channel + "' to the sound system");
  }

  //-------------------------------------------------------------------------------------------
  void SoundSystem::Update()
  {
    fmod_system_->update();
  }

  //-------------------------------------------------------------------------------------------
  FMOD::Sound* SoundSystem::Load(const std::string& path)
  {
    FMOD::Sound* snd = nullptr;
    FMOD_RESULT result;

    result = fmod_system_->createSound((Game::Instance()->path() + "/" + path).c_str(), FMOD_DEFAULT, 0, &snd);
    SNUFF_XASSERT(result == FMOD_OK, "Failed creating sound '" + path + "'", "SoundSystem::Load::" + path);

    return snd;
  }

  //-------------------------------------------------------------------------------------------
  void SoundSystem::Play(const std::string& path, const std::string& channel_group, const bool& loop)
  {
    Sound* sound = ContentManager::Instance()->Get<Sound>(path);

    if (sound == nullptr)
    {
      return;
    }

    std::map<std::string, FMOD::ChannelGroup*>::iterator it = channel_groups_.find(channel_group);

    if (it == channel_groups_.end())
    {
      SNUFF_LOG_ERROR("Channel group attempted to play sound on does not exist! Channel: '" + channel_group + "'");
      return;
    }

    loop == true ? sound->get()->setMode(FMOD_LOOP_NORMAL) : sound->get()->setMode(FMOD_LOOP_OFF);
    fmod_system_->playSound(sound->get(), it->second, false, 0);
  }

  //-------------------------------------------------------------------------------------------
  void SoundSystem::SetChannelGroupVolume(const std::string& channel_group, const float& volume)
  {
    std::map<std::string, FMOD::ChannelGroup*>::iterator it = channel_groups_.find(channel_group);

    if (it == channel_groups_.end())
    {
      SNUFF_LOG_ERROR("Channel group attempted to play sound on does not exist! Channel: '" + channel_group + "'");
      return;
    }

    float v = volume;
    if (v < 0.0f || v > 1.0f)
    {
      SNUFF_LOG_WARNING("Volume should be in a 0.0 to 1.0 range, clamping");
      v = std::fminf(v, 1.0f);
      v = std::fmaxf(v, 0.0f);
    }

    it->second->setVolume(v);
  }

  //-------------------------------------------------------------------------------------------
  void SoundSystem::Stop(const std::string& channel_group)
  {
    std::map<std::string, FMOD::ChannelGroup*>::iterator it = channel_groups_.find(channel_group);

    if (it == channel_groups_.end())
    {
      SNUFF_LOG_ERROR("Channel group attempted to stop sound on does not exist! Channel: '" + channel_group + "'");
      return;
    }

    it->second->stop();
  }

  //-------------------------------------------------------------------------------------------
  void SoundSystem::Pause(const std::string& channel_group)
  {
    std::map<std::string, FMOD::ChannelGroup*>::iterator it = channel_groups_.find(channel_group);

    if (it == channel_groups_.end())
    {
      SNUFF_LOG_ERROR("Channel group attempted to pause sound on does not exist! Channel: '" + channel_group + "'");
      return;
    }

    it->second->setPaused(true);
  }

  //-------------------------------------------------------------------------------------------
  void SoundSystem::Resume(const std::string& channel_group)
  {
    std::map<std::string, FMOD::ChannelGroup*>::iterator it = channel_groups_.find(channel_group);

    if (it == channel_groups_.end())
    {
      SNUFF_LOG_ERROR("Channel group attempted to resume sound on does not exist! Channel: '" + channel_group + "'");
      return;
    }

    it->second->setPaused(false);
  }

  //-------------------------------------------------------------------------------------------
  bool SoundSystem::IsPaused(const std::string& channel_group)
  {
    std::map<std::string, FMOD::ChannelGroup*>::iterator it = channel_groups_.find(channel_group);

    if (it == channel_groups_.end())
    {
      SNUFF_LOG_ERROR("Channel group attempted to check paused on does not exist! Channel: '" + channel_group + "'");
      return false;
    }

    bool paused = false;
    it->second->getPaused(&paused);
    return paused;
  }

  //-------------------------------------------------------------------------------------------
  void SoundSystem::JSResume(JS_ARGS args)
  {
    JSWrapper wrapper(args);

    if (wrapper.Check("S") == true)
    {
      SoundSystem::Instance()->Resume(wrapper.GetValue<std::string>(0, "undefined"));
    }
  }

  //-------------------------------------------------------------------------------------------
  void SoundSystem::JSIsPaused(JS_ARGS args)
  {
    JSWrapper wrapper(args);

    if (wrapper.Check("S") == true)
    {
      wrapper.ReturnValue<bool>(SoundSystem::Instance()->IsPaused(wrapper.GetValue<std::string>(0, "undefined")));
    }
  }

  //-------------------------------------------------------------------------------------------
  void SoundSystem::JSStop(JS_ARGS args)
  {
    JSWrapper wrapper(args);

    if (wrapper.Check("S") == true)
    {
      SoundSystem::Instance()->Stop(wrapper.GetValue<std::string>(0, "undefined"));
    }
  }

  //-------------------------------------------------------------------------------------------
  void SoundSystem::JSPause(JS_ARGS args)
  {
    JSWrapper wrapper(args);

    if (wrapper.Check("S") == true)
    {
      SoundSystem::Instance()->Pause(wrapper.GetValue<std::string>(0, "undefined"));
    }
  }

  //-------------------------------------------------------------------------------------------
  void SoundSystem::JSPlay(JS_ARGS args)
  {
    JSWrapper wrapper(args);

    if (wrapper.Check("SS") == true)
    {
      SoundSystem::Instance()->Play(
        wrapper.GetValue<std::string>(0, "undefined"), 
        wrapper.GetValue<std::string>(1, "undefined"),
        wrapper.GetValue<bool>(2, false));
    }
  }

  //-------------------------------------------------------------------------------------------
  void SoundSystem::JSAddChannelGroup(JS_ARGS args)
  {
    JSWrapper wrapper(args);

    if (wrapper.Check("S") == true)
    {
      SoundSystem::Instance()->AddChannelGroup(wrapper.GetValue<std::string>(0, "undefined"));
    }
  }

  //-------------------------------------------------------------------------------------------
  void SoundSystem::JSSetChannelGroupVolume(JS_ARGS args)
  {
    JSWrapper wrapper(args);

    if (wrapper.Check("SN") == true)
    {
      SoundSystem::Instance()->SetChannelGroupVolume(wrapper.GetValue<std::string>(0, "undefined"), wrapper.GetValue<float>(1, 1.0f));
    }
  }

  //-------------------------------------------------------------------------------------------
  void SoundSystem::RegisterJS(JS_SINGLETON obj)
  {
    JSFunctionRegister funcs[] = {
      { "play", JSPlay },
      { "pause", JSPause },
      { "stop", JSStop },
      { "resume", JSResume },
      { "isPaused", JSIsPaused },
      { "addChannelGroup", JSAddChannelGroup },
      { "setChannelGroupVolume", JSSetChannelGroupVolume }
    };

    JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
  }

  //-------------------------------------------------------------------------------------------
  SoundSystem::~SoundSystem()
  {
    SNUFF_LOG_INFO("Shutting down the FMOD sound system");
    fmod_system_->release();
    SNUFF_LOG_INFO("Shut down the FMOD sound system");

    is_available_ = false;
  }
}