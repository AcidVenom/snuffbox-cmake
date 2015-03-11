#pragma once

namespace snuffbox
{
	class D3D11RenderElement;

	/**
	* @class snuffbox::Animation
	* @brief The base interface class for every animation
	* @author Daniël Konings
	*/
	class Animation
	{
	public:
		/**
		* @brief Applies the animation
		* @param[in] dt (const float&) The current delta time
		*/
		virtual void Animate(const float& dt) = 0;

		/// Plays the animation
		virtual void Play() = 0;

		/// Pauses the animation
		virtual void Pause() = 0;

		/// Stops the animation
		virtual void Stop() = 0;

		/// Is the animation playing?
		virtual const bool& IsPlaying() = 0;

		/**
		* @brief Sets the parent of this animation
		* @param[in] parent (snuffbox::D3D11RenderElement*) The parent to set
		*/
		virtual void set_parent(D3D11RenderElement* parent) = 0;
	};
}