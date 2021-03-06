#pragma once

#include "../input/key.h"
#include <queue>

#include "../js/js_object.h"

namespace snuffbox
{
	/**
	* @class snuffbox::Keyboard
	* @brief This class handles all keyboard input
	* @author Dani�l Konings
	*/
	class Keyboard : public JSObject
	{
	public:
		/**
		* @enum snuffbox::Keyboard::KeyEvent
		* @brief An enumeration that contains different key events
		* @author Dani�l Konings
		*/
		enum KeyEvent
		{
			kPressed,
			kReleased
		};

		/**
		* @enum snuffbox::Keyboard::KeyData
		* @brief Contains key data sent by the window
		* @author Dani�l Konings
		*/
		struct KeyData
		{
			KeyEvent evt;
			Key::Keys keycode;
		};

		/**
		* @struct snuffbox::Keyboard::KeyState
		* @brief Contains information about a key's current state
		* @author Dani�l Konings
		*/
		struct KeyState
		{
			/// Initialises the default state
			KeyState() : down(false), pressed(false), released(false){}

			bool down; //!< Was this key down last frame?
			bool pressed; //!< Was this key pressed last frame?
			bool released; //!< Was this key released last frame?
		};

	public:
		/// Default constructor
		Keyboard();

		/// Default destructor
		~Keyboard();

    /**
    * @brief Returns the singleton instance of this object
    * @return snuffbox::Keyboard* The pointer to the singleton
    */
    static Keyboard* Instance();

		/// Reset all key states
		void ResetStates();

		/**
		* @brief Notify the keyboard about a change in input
		* @param[in] data (const snuffbox::Keyboard::Keydata&) The data sent from the window
		*/
		void Notify(const KeyData& data);

		/// Updates the keyboard
		void Update();

		/**
		* @brief Is a given key pressed?
		* @param[in] keycode (const snuffbox::Key::Keys&) The keycode of the key to be checked
		* @return const bool& The boolean value
		*/
		const bool& IsPressed(const Key::Keys& keycode) const;

		/**
		* @brief Is a given key down?
		* @param[in] keycode (const snuffbox::Key::Keys&) The keycode of the key to be checked
		* @return const bool& The boolean value
		*/
		const bool& IsDown(const Key::Keys& keycode) const;

		/**
		* @brief Is a given key released?
		* @param[in] keycode (const snuffbox::Key::Keys&) The keycode of the key to be checked
		* @return const bool& The boolean value
		*/
		const bool& IsReleased(const Key::Keys& keycode) const;

		/**
		* @return const snuffbox::Key::Keys& The last key pressed on the keyboard
		*/
		const Key::Keys& last_pressed() const;

		/**
		* @return const snuffbox::Key::Keys& The last key released off of the keyboard
		*/
		const Key::Keys& last_released() const;

	private:
		std::queue<KeyData> queue_; //!< The message queue that still needs processing
		KeyState						states_[255]; //!< A list of key states
		Key::Keys						last_pressed_; //!< The key last pressed
		Key::Keys						last_released_; //!< The key last released

    /// Enumerates the key enumerator, sets it all to JavaScript
    static void JSEnumerateKeys();

  public:
    JS_NAME("Keyboard");
    static void RegisterJS(JS_SINGLETON obj);
    static void JSIsPressed(JS_ARGS args);
    static void JSIsDown(JS_ARGS args);
    static void JSIsReleased(JS_ARGS args);
		static void JSLastPressed(JS_ARGS args);
		static void JSLastReleased(JS_ARGS args);
	};
}