#pragma once

#include "../js/js_object.h"
#include "../d3d11/d3d11_render_device.h"

namespace snuffbox
{
	class D3D11RenderSettings : public JSObject
	{
	public:
		/// Default constructor
		D3D11RenderSettings();

		/**
		* @brief Retrieves the singleton instance of this class
		* @return snuffbox::D3D11RenderSettings* The pointer to the singleton
		*/
		static D3D11RenderSettings* Instance();

		/**
		* @return const bool& Is vertical sync enabled?
		*/
		const bool& vsync() const;

		/**
		* @return const XMFLOAT2& The resolution of the renderer
		*/
		const XMFLOAT2& resolution() const;

		/**
		* @brief Sets if vsync is enabled or not
		* @param[in] vsync (const bool&) The boolean value
		*/
		void set_vsync(const bool& vsync);

		/**
		* @brief Sets the resolution of the renderer
		* @param[in] width (const float&) The new width
		* @param[in] height (const float&) The new height
		*/
		void set_resolution(const float& width, const float& height);

		/// Default destructor
		~D3D11RenderSettings();

	private:
		bool vsync_; //!< Is vertical sync enabled?
		XMFLOAT2 resolution_; //!< The resolution of the renderer

	public:
    JS_NAME("RenderSettings");
		static void RegisterJS(JS_SINGLETON obj);
		static void JSSetVsync(JS_ARGS args);
		static void JSVsync(JS_ARGS args);
		static void JSSetResolution(JS_ARGS args);
		static void JSResolution(JS_ARGS args);
	};
}