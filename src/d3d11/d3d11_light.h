#pragma once

#include "../js/js_object.h"
#include "../d3d11/d3d11_render_device.h"

namespace snuffbox
{
	/**
	* @class snuffbox::D3D11Light
	* @brief A light class that can define a point, directional and spot light
	* @author Daniël Konings
	*/
	class D3D11Light : public JSObject
	{
	public:
		/**
		* @enum snuffbox::D3D11Light::LightTypes
		* @brief Contains enumerators for the different available light types
		* @author Daniël Konings
		*/
		enum LightTypes
		{
			kPoint,
			kDirectional,
			kSpot
		};

		/**
		* @enum snuffbox::D3D11Light::Attributes
		* @brief Different light attributes to adjust lights
		* @author Daniël Konings
		*/
		__declspec(align(16)) struct Attributes
		{
			XMFLOAT4 translation;
			XMFLOAT4 direction;
			XMFLOAT4 colour;

			float spot_angle;
			float radius;

			int type;
			bool activated;
		};
	public:
		
		/**
		* @brief Constructs a light by type
		* @param[in] type (D3D11Light::LightTypes) The type to create this light with
		*/
		D3D11Light(LightTypes type);

		/// JavaScript constructor
		D3D11Light(JS_ARGS args);

		/// Defaults the attributes
		void Default();

		/**
		* @brief Translates the light by a given value
		* @param[in] x (const float&) The movement on the x-axis
		* @param[in] y (const float&) The movement on the y-axis
		* @param[in] z (const float&) The movement on the z-axis
		*/
		void TranslateBy(const float& x, const float& y, const float& z);

		/**
		* @return const snuffbox::D3D11Light::Attributes& The attributes of this light
		*/
		const Attributes& attributes() const;

		/**
		* @brief Sets the translation of this light
		* @param[in] x (const float&) The new x position
		* @param[in] y (const float&) The new y position
		* @param[in] z (const float&) The new z position
		*/
		void set_translation(const float& x, const float& y, const float& z);

		/**
		* @brief Sets the direction of this light
		* @param[in] x (const float&) The new x direction
		* @param[in] y (const float&) The new y direction
		* @param[in] z (const float&) The new z direction
		*/
		void set_direction(const float& x, const float& y, const float& z);

		/**
		* @brief Sets the emitted colour of this light
		* @param[in] r (const float&) The new red value
		* @param[in] g (const float&) The new green value
		* @param[in] b (const float&) The new blue value
		* @param[in] a (const float&) The new alpha, default = 0.0f
		*/
		void set_colour(const float& r, const float& g, const float& b, const float& a = 0.0f);

		/**
		* @brief Sets the spot angle of this light, if it's a spot light
		* @param[in] angle (const float&) The angle to set
		*/
		void set_spot_angle(const float& angle);

		/**
		* @brief Sets the radius of this light, if it's a point light
		* @param[in] r (const float&) The new radius
		*/
		void set_radius(const float& r);

		/**
		* @brief Enables or disables this light
		* @param[in] activated (const bool&) The new value to set
		*/
		void set_activated(const bool& activated);

		/// Default destructor
		virtual ~D3D11Light();

	private:
		Attributes attributes_; //!< The attributes of this light

	public:
		JS_NAME("Light");
		static void JSEnumerate();
		static void RegisterJS(JS_CONSTRUCTABLE obj);
		static void JSSetTranslation(JS_ARGS args);
		static void JSTranslateBy(JS_ARGS args);
		static void JSTranslation(JS_ARGS args);
		static void JSSetDirection(JS_ARGS args);
		static void JSDirection(JS_ARGS args);
		static void JSSetColour(JS_ARGS args);
		static void JSColour(JS_ARGS args);
		static void JSSetSpotAngle(JS_ARGS args);
		static void JSSpotAngle(JS_ARGS args);
		static void JSSetRadius(JS_ARGS args);
		static void JSRadius(JS_ARGS args);
		static void JSSetActivated(JS_ARGS args);
		static void JSActivated(JS_ARGS args);
	};
}