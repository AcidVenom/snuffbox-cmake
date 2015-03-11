#pragma once

#include "../content/content.h"
#include "../animation/sprite_animation.h"

namespace snuffbox
{
	/**
	* @class snuffbox::Anim
	* @brief An animation file to be used with sprite sheet animation
	* @author Daniël Konings
	*/
	class Anim : public Content
	{
	public:

		/// Default constructor
		Anim();

		/**
		* @brief Loads the anim file from a given path and parses it
		* @param[in] path (const std::string&) The relative path
		*/
		void Load(const std::string& path);

		/**
		* @brief Fills a frame with frame information
		* @param[in] obj (const v8::Handle<v8::Object>&) The description to use to fill the frame with
		* @param[out] frame (SpriteAnimation::Frame*) The frame to fill
		* @param[in] i (const int&) The current index
		* @param[in] path (const std::string&) The current path
		*/
		void FillFrame(const v8::Handle<v8::Object>& obj, SpriteAnimation::Frame* frame, const int& i, const std::string& path);

		/**
		* @brief Retrieves a frame by index
		* @param[in] frame (const int&) The frame to retrieve
		* @return snuffbox::SpriteAnimation::Frame* The actual frame, nullptr if it doesn't exist
		*/
		SpriteAnimation::Frame* GetFrame(const int& frame);

		/**
		* @return int The number of frames of this animation
		*/
		int NumFrames();

		/// Default destructor
		virtual ~Anim();

	private:
		std::vector<SpriteAnimation::Frame> frames_; //!< The frames of this anim
		bool valid_; //!< Is this animation valid?
	};
}