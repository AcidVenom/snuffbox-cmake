#pragma once

#include "../animation/animation_base.h"
#include "../js/js_object.h"

#include <vector>

namespace snuffbox
{
	class Anim;
	class D3D11RenderElement;
	class D3D11Texture;

	/**
	* @class snuffbox::SpriteAnimation
	* @brief Used with sprite sheets to animate through them
	* @author Daniël Konings
	*/
	class SpriteAnimation : public Animation, public JSObject
	{
	public:

		/**
		* @struct snuffbox::SpriteAnimation::Frame
		* @brief Contains frame information for the sprite animation to process
		* @author Daniël Konings
		*/
		struct Frame
		{
			int x, y, w, h;
			float wait;
		};

	public:

		/// Default constructor
		SpriteAnimation();

		/// JavaScript constructor
		SpriteAnimation(JS_ARGS args);

		/// @see snuffbox::IAnimationBase::Animate
		void Animate(const float& dt);

		/// @see snuffbox::IAnimationBase::Play
		void Play();

		/// @see snuffbox::IAnimationBase::Pause
		void Pause();

		/// @see snuffbox::IAnimationBase::Stop
		void Stop();

		/// @see snuffbox::IAnimationBase::IsPlaying
		const bool& IsPlaying();

		/**
		* @brief Calculates the frame time to the current frame
		* @param[in] i (int) The current frame index
		* @return float The total time
		*/
		float CalculateFrameTime(int i);

		/**
		* @return const int& The current frame of this animation
		*/
		const int& current_frame() const;

		/**
		* @return const float& The current speed of this animation
		*/
		const float& speed() const;

		/**
		* @brief Sets the speed of this animation
		* @param[in] speed (const float&) The new speed
		*/
		void set_speed(const float& speed);

		/**
		* @brief Sets the current frame
		* @param[in] frame (const int&) The frame to set the animation to
		*/
		void set_current_frame(const int& frame);

		/**
		* @brief Sets the parent of this animation
		* @param[in] element (snuffbox::D3D11RenderElement*) The new parent
		*/
		void set_parent(D3D11RenderElement* element);

		/// Default destructor
		virtual ~SpriteAnimation();

	private:
		Anim* anim_; //!< The animation file that this sprite animation uses
		bool is_playing_; //!< Is this animation playing?
		int current_frame_; //!< The current frame of this sprite animation
		float speed_; //!< The speed of this sprite animation
		float elapsed_time_; //!< The elapsed time of this sprite animation
		D3D11RenderElement* parent_; //!< The parent of this sprite animation
		D3D11Texture* texture_; //!< The texture this animation uses

	public:
		JS_NAME("SpriteAnimation");
		static void RegisterJS(JS_CONSTRUCTABLE obj);
		static void JSPlay(JS_ARGS args);
		static void JSPause(JS_ARGS args);
		static void JSStop(JS_ARGS args);
		static void JSIsPlaying(JS_ARGS args);
		static void JSSetFrame(JS_ARGS args);
		static void JSCurrentFrame(JS_ARGS args);
		static void JSSetSpeed(JS_ARGS args);
		static void JSSpeed(JS_ARGS args);
	};
}