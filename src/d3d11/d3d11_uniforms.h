#pragma once

#include <string>
#include <map>

#include "../d3d11/d3d11_render_device.h"
#include "../js/js_object.h"

namespace snuffbox
{
	/**
	* @class snuffbox::D3D11Uniforms
	* @brief A class to manage uniforms to be sent to the shader
	* @author Daniël Konings
	*/
	class D3D11Uniforms : public JSObject
	{
	public:
		/**
		* @struct snuffbox::D3D11Uniforms::UniformTypes
		* @brief Contains an enumeration for the different types of uniforms that can be set, these map to the number of values
		* @author Daniël Konings
		*/
		enum UniformTypes : int
		{
			kFloat = 1,
			kFloat2 = 2,
			kFloat3 = 3,
			kFloat4 = 4
		};

	public:
		/// Default constructor
		D3D11Uniforms();

		/**
		* @brief Sets a uniform value
		* @param[in] type (const snuffbox::D3D11Uniforms::UniformTypes&) The type of uniform to set
		* @param[in] name (const std::string&) The name of the uniform to set
		* @param[in] value (const float*) A buffer of values
		*/
		void SetUniform(const UniformTypes& type, const std::string& name, const float* value);

		/**
		* @brief Sets the value of a float4
		* @param[in] type (const snuffbox::D3D11Uniforms::UniformTypes&) The type of the uniform value to set
		* @param[in] v (XMFLOAT4&) A reference to the float4 to set
		* @param[in] value (const float*) The actual value to set
		*/
		void SetValue(const UniformTypes& type, XMFLOAT4& v, const float* value);

		/// Applies the uniform buffer
		void Apply();

		/// Default destructor
		virtual ~D3D11Uniforms();

	private:
		typedef std::map<std::string, XMFLOAT4> Uniforms;
		Uniforms uniforms_; //!< The actual uniforms

	public:
		JS_NAME("Uniform");
		static void RegisterJS(JS_SINGLETON obj);
	};
}