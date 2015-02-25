#pragma once

#include <queue>

#include "../js/js_object.h"

namespace snuffbox
{
	/**
	* @class snuffbox::Mouse
	* @brief This class handles all mouse input
	* @author Daniël Konings
	*/
	class Mouse : public JSObject
	{
	public:
		/**
		* @enum snuffbox::Mouse::MouseEvent
		* @brief An enumeration that contains different mouse events
		* @author Daniël Konings
		*/
		enum MouseEvent
		{
			kPressed,
			kReleased,
			kWheelDown,
			kWheelUp,
			kDblClk,
			kMove
		};

		/**
		* @enum snuffbox::Mouse::MouseButton
		* @brief An enumeration for the different mouse buttons that exist
		* @author Daniël Konings
		*/
		enum MouseButton
		{
			kLeft,
			kRight,
			kMiddle
		};

		/**
		* @enum snuffbox::Mouse::MouseData
		* @brief Contains mouse data sent by the window
		* @author Daniël Konings
		*/
		struct MouseData
		{
			MouseEvent evt;
			MouseButton button;
			int x;
			int y;
		};

		/**
		* @struct snuffbox::Mouse::ButtonState
		* @brief Contains information about a button's current state
		* @author Daniël Konings
		*/
		struct ButtonState
		{
			/// Initialises the default state
			ButtonState() : down(false), pressed(false), released(false), double_clicked(false){}

			bool down; //!< Was this button down last frame?
			bool pressed; //!< Was this button pressed last frame?
			bool released; //!< Was this button released last frame?
			bool double_clicked; //!< Was this button double clicked last frame?
		};

	public:
		/// Default constructor
		Mouse();

		/// Default destructor
		~Mouse();

    /**
    * @brief Returns the singleton instance of this object
    * @return snuffbox::Mouse* The pointer to the singleton
    */
    static Mouse* Instance();

		/// Reset all button states
		void ResetStates();

		/**
		* @brief Notify the mouse about a change in input
		* @param[in] data (snuffbox::Mouse::MouseData) The data sent from the window
		*/
		void Notify(MouseData data);

		/// Updates the mouse
		void Update();

		/**
		* @brief Is a given button pressed?
		* @param[in] button (const snuffbox::Mouse::MouseButton&) The button ID of the button to be checked
		* @return const bool& The boolean value
		*/
		const bool& IsPressed(const MouseButton& button) const;

		/**
		* @brief Is a given button down?
		* @param[in] button (const snuffbox::Mouse::MouseButton&) The button ID of the button to be checked
		* @return const bool& The boolean value
		*/
		const bool& IsDown(const MouseButton& button) const;

		/**
		* @brief Is a given button released?
		* @param[in] button (const snuffbox::Mouse::MouseButton&) The button ID of the button to be checked
		* @return const bool& The boolean value
		*/
		const bool& IsReleased(const MouseButton& button) const;

		/**
		* @brief Is a given button double clicked?
		* @param[in] button (const snuffbox::Mouse::MouseButton&) The button ID of the button to be checked
		* @return const bool& The boolean value
		*/
		const bool& IsDoubleClicked(const MouseButton& button) const;

		/**
		* @brief Was there a mouse wheel down?
		* @return const bool& The boolean value
		*/
		const bool& wheel_down() const;

		/**
		* @brief Was there a mouse wheel up?
		* @return const bool& The boolean value
		*/
		const bool& wheel_up() const;

		/**
		* @return const int& The x position of the mouse
		*/
		const int& x() const;

		/**
		* @return const int& The y position of the mouse
		*/
		const int& y() const;

		/**
		* @return const int& The x movement of the mouse
		*/
		const int& dx() const;

		/**
		* @return const int& The y movement of the mouse
		*/
		const int& dy() const;

	private:
		std::queue<MouseData> queue_; //!< The message queue that still needs processing
		ButtonState						states_[3]; //!< A list of button states
		bool									wheel_down_; //!< Was the mouse wheel scrolled down?
		bool									wheel_up_; //!< Was the mouse wheel scrolled up?
		int										x_; //!< The x position of the mouse
		int										y_; //!< The y position of the mouse
		int										dx_; //!< The x movement since last frame
		int										dy_; //!< The y movement since last frame
		int										prev_x_; //!< The previous x position
		int										prev_y_; //!< The previous y position

		/// Enumerates all buttons to a 'Button' enumeration
		static void JSEnumerateButtons();
	public:
		JS_NAME("Mouse");
		static void RegisterJS(JS_SINGLETON obj);
		static void JSIsDown(JS_ARGS args);
		static void JSIsPressed(JS_ARGS args);
		static void JSIsReleased(JS_ARGS args);
		static void JSIsDoubleClicked(JS_ARGS args);
		static void JSWheelDown(JS_ARGS args);
		static void JSWheelUp(JS_ARGS args);
		static void JSPosition(JS_ARGS args);
		static void JSMovement(JS_ARGS args);
	};
}