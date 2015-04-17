#pragma once

#include <fmod.hpp>

#include "../js/js_object.h"

namespace snuffbox
{
  class Sound;

  /**
  * @class snuffbox::SoundSystem
  * @brief The sound system that handles all sound events, sound banks, etc. of FMOD
  * @author Daniël Konings
  */
  class SoundSystem : public JSObject
  {
  public:

    /// Default constructor
    SoundSystem();

    /// Default destructor
    virtual ~SoundSystem();

    /**
    * @brief Retrieves the singleton instance of this class
    * @return snuffbox::SoundSystem* The pointer to the singleton instance
    */
    static SoundSystem* Instance();

    /// Updates the sound system
    void Update();

    /**
    * @brief Loads a sound from a path and returns it
    * @param[in] path (const std::string&) The path to load the sound from
    * @return FMOD::Sound* A pointer to the loaded sound
    */
    FMOD::Sound* Load(const std::string& path);

    /**
    * @brief Plays a sound
    * @param[in] path (const std::string&) The path to the sound to play
    * @param[in] channel_group (const std::string&) The channel group to play the sound on
    * @param[in] loop (const bool&) Should this sound loop?
    */
    void Play(const std::string& path, const std::string& channel_group, const bool& loop = false);

    /**
    * @brief Adds a channel group
    * @param[in] channel_group (const std::string&) The name of the channel group that will be added
    */
    void AddChannelGroup(const std::string& channel_group);

    /**
    * @brief Sets the volume of a channel group
    * @param[in] channel_group (const std::string&) The name of the channel group to set the volume of
    * @param[in] volume (const float&) The new volume
    */
    void SetChannelGroupVolume(const std::string& channel_group, const float& volume);

    /**
    * @brief Stops a channel group
    * @param[in] channel_group (const std::string&) The name of the channel group to stop
    */
    void Stop(const std::string& channel_group);

    /**
    * @brief Pauses a channel group
    * @param[in] channel_group (const std::string&) The name of the channel group to pause
    */
    void Pause(const std::string& channel_group);

    /**
    * @brief Resumes a channel group
    * @param[in] channel_group (const std::string&) The name of the channel group to resume
    */
    void Resume(const std::string& channel_group);

    /**
    * @brief Is a given channel group paused?
    * @param[in] channel_group (const std::string&) The name of the channel group to check
    * @return bool Is the channel group paused?
    */
    bool IsPaused(const std::string& channel_group);

  private:
    FMOD::System* fmod_system_; //!< The actual FMOD system
    std::map<std::string, FMOD::ChannelGroup*> channel_groups_; //!< All channel groups

  public:
    JS_NAME("SoundSystem");

    static void RegisterJS(JS_SINGLETON obj);
    static void JSPlay(JS_ARGS args);
    static void JSPause(JS_ARGS args);
    static void JSIsPaused(JS_ARGS args);
    static void JSResume(JS_ARGS args);
    static void JSStop(JS_ARGS args);
    static void JSAddChannelGroup(JS_ARGS args);
    static void JSSetChannelGroupVolume(JS_ARGS args);

    /**
    * @return const bool& Is the sound system available?
    */
    static const bool& IsAvailable(); 
  };
}