#pragma once

#include "../d3d11/d3d11_render_device.h"
#include "../js/js_object.h"
#include <vector>

namespace snuffbox
{
	class D3D11Light;

	/**
	* @class snuffbox::D3D11Lighting
	* @brief The lighting environment to adjust global illumination settings and to store lights in
	* @author Daniël Konings
	*/
	class D3D11Lighting : public JSObject
	{
	public:

		/// Default constructor
		D3D11Lighting();

		/**
		* @brief Retrieves the singleton instance of this class
		* @return snuffbox::D3D11Lighting* The pointer to the singleton
		*/
		static D3D11Lighting* Instance();

		/**
		* @brief Adds a given light
		* @param[in] light (snuffbox::D3D11Light*) The pointer to the light to add
		*/
		void Add(D3D11Light* light);

		/**
		* @brief Removes a given light
		* @param[in] light (snuffbox::D3D11Light*) The pointer to the light to remove
		*/
		void Remove(D3D11Light* light);

		/**
		* @brief Updates the lighting system
		* @param[in] buffer (snuffbox::D3D11ConstantBuffer*) The constant buffer to fill
		*/
		void Update(D3D11ConstantBuffer* buffer);

		/**
		* @return const std::vecotr<snuffbox::D3D11Light*>& The vector of lights currently activated
		*/
		const std::vector<D3D11Light*>& lights() const;

		/**
		* @return const XMFLOAT4& The ambient colour that is currently in use
		*/
		const XMFLOAT4& ambient_colour() const;

		/**
		* @brief Sets the ambient colour
		* @param[in] r (const float&) The red value of the colour
		* @param[in] g (const float&) The green value of the colour
		* @param[in] b (const float&) The blue value of the colour
		*/
		void set_ambient_colour(const float& r, const float& g, const float& b);

		/// Default destructor
		~D3D11Lighting();

	private:
		std::vector<D3D11Light*> lights_; //!< The list of lights that need to be passed to the constant buffer
		XMFLOAT4 ambient_colour_; //!< The global ambient colour

	public:
		JS_NAME("Lighting");
		static void RegisterJS(JS_SINGLETON obj);
		static void JSSetAmbientColour(JS_ARGS args);
		static void JSAmbientColour(JS_ARGS args);
	};
}