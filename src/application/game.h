#pragma once

#include "../js/js_callback.h"

namespace snuffbox
{
	class Window;
	class Keyboard;
	class Mouse;

	class Game
	{
	public:
		/**
		* @enum snuffbox::GameNotifications
		* @brief An enumerator that contains different notifcations the game can retrieve externally
		*/
		enum GameNotifications
		{
			kQuit,
			kReload
		};

	public:
		/// Default constructor
		Game();

		/**
		* @brief Retrieves the singleton instance of the game object
		* @return snuffbox::Game* The singleton instance
		*/
		static Game* Instance();

		/// Verifies if all necessary environment variables have been set
		void Verify();

		/// Runs this instance of the engine
		void Run();

		/**
		* @brief Sends a notification to the game instance
		* @param[in] evt (snuffbox::Game::GameNotifications) The notification to send
		*/
		void Notify(GameNotifications evt);

		/// Quits the engine
		void Quit();

		/**
		* @return const bool& Is the engine running?
		*/
		const bool& started() const;

		/**
		* @return snuffbox::Window* The window associated with this instance of the engine
		*/
		Window* window();

		/**
		* @return snuffbox::Keyboard* The keyboard associated with this instance of the engine
		*/
		Keyboard* keyboard();

		/**
		* @return snuffbox::Mouse* The mouse associated with this instance of the engine
		*/
		Mouse* mouse();

		/**
		* @brief Sets the window of this object
		* @param[in] window (snuffbox::Window*) The window to be associated with this instance of the engine
		*/
		void set_window(Window* window);

		/**
		* @brief Sets the keyboard of this object
		* @param[in] keyboard (snuffbox::Keyboard*) The keyboard to be associated with this instance of the engine
		*/
		void set_keyboard(Keyboard* keyboard);

		/**
		* @brief Sets the mouse of this object
		* @param[in] mouse (snuffbox::Mouse*) The mouse to be associated with this instance of the engine
		*/
		void set_mouse(Mouse* mouse);

		/// Default destructor
		~Game();

	private:
		Window* window_;
		Keyboard* keyboard_;
		Mouse* mouse_;
		bool started_;

  public:
    JSCallback<> js_init_; //!< The initialisation callback
    JSCallback<double> js_update_;  //!< The update callback
	};
}