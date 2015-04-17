#pragma once

#include "../fmod/fmod_sound_system.h"
#include "../content/content.h"

namespace snuffbox
{
  class Sound : public Content
  {
  public:
    /// Default constructor
    Sound();

    /// @see snuffbox::Content
    void Load(const std::string& path);

    /// Default destructor
    virtual ~Sound();

    /// Returns the actual sound
    FMOD::Sound* get();

  private:
    FMOD::Sound* sound_; //!< The actual sound
  };
}