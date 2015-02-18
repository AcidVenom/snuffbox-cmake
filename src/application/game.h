#pragma once

#include "../js/js_callback.h"
#include "../js/js_object.h"

#include <chrono>

namespace snuffbox
{
	class Window;
	class Keyboard;
	class Mouse;

	class Game : public JSObject
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

		/// Initialises the game
		void Initialise();

		/// Updates the input
		void UpdateInput();
		
		/// Calls the update functions
		void Update();

		/// Calls the fixed update functions
		void FixedUpdate();

		/// Updates the console
		void UpdateConsole();

		/// Runs this instance of the engine
		void Run();

		/// Reloads the game
		void Reload();

		/**
		* @brief Sends a notification to the game instance
		* @param[in] evt (snuffbox::Game::GameNotifications) The notification to send
		*/
		void Notify(GameNotifications evt);

		/// Quits the engine
		void Quit();

		/**
		* @return std::string& The path the engine is running in
		*/
		const std::string& path() const;

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
		* @return const double& The current fixed time step
		*/
		const double& fixed_step() const;

		/**
		* @return const double& The current game time
		*/
		const double& time() const;

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

		/**
		* @brief Sets the fixed time step
		* @param[in] step (double) The fixed time step to set
		*/
		void set_fixed_step(double step);

		/**
		* @brief Sets the current game time
		* @param[in] time (double) The time to set
		*/
		void set_time(double time);

		/// Default destructor
		~Game();

	private:
		std::string path_; //!< The path the game is running in
		Window* window_; //!< The window the game is running in
		Keyboard* keyboard_; //!< The keyboard the game currently uses
		Mouse* mouse_; //!< The mouse the game currently uses
		bool started_; //!< Is the game started?
		double delta_time_; //!< The delta time since the previous frame
		std::chrono::high_resolution_clock::time_point last_time_; //!< What was the time the last time?
		double fixed_step_; //!< The current fixed time step
		double left_over_delta_; //!< The unused delta time since last frame
		double accumulated_time_; //!< The total accumulated time since last frame
		double time_; //!< The current game time

    JSCallback<> js_init_; //!< The initialisation callback
    JSCallback<double> js_update_;  //!< The update callback
		JSCallback<int, double> js_fixed_update_; //!< The fixed update callback
		JSCallback<> js_shutdown_; //!< The shutdown callback
		JSCallback<std::string> js_on_reload_; //!< The on-reload callback

	public:
		JS_NAME("Game");
		static void RegisterJS(JS_SINGLETON obj);
		static void JSQuit(JS_ARGS args);
		static void JSTime(JS_ARGS args);
		static void JSSetTime(JS_ARGS args);
		static void JSFixedStep(JS_ARGS args);
		static void JSSetFixedStep(JS_ARGS args);
	};
}