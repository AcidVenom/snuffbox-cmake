#pragma once

#include "../d3d11/d3d11_render_device.h"
#include "../d3d11/d3d11_sampler_state.h"
#include "../content/content.h"
#include <vector>

namespace snuffbox
{
	class D3D11Shader;

	class D3D11Effect : public Content
	{
	public:

		/**
		* @struct snuffbox::D3D11Effect::Pass
		* @brief Information for a shader pass
		* @author Daniël Konings
		*/
		struct Pass
		{
			D3D11Shader* shader;
			SharedPtr<D3D11BlendState> blend_state;
			SharedPtr<D3D11DepthState> depth_state;
			D3D11SamplerState::SamplerTypes sampling;
		};

		/**
		* @struct snuffbox::D3D11Effect::Technique
		* @brief Contains all values used for a shader technique
		* @author Daniël Konings
		*/
		struct Technique
		{
			std::string name;
			std::vector<Pass> passes;
		};

	public:
		/// Default constructor
		D3D11Effect();

		/**
		* @brief Loads this effect from a given JSON
		* @param[in] path (const std::string&) The path to the JSON
		*/
		void Load(const std::string& path);

		/**
		* @brief Applies this effect
		* @param[in] tech (const std::string&) The name of the technique to apply
		* @param[in] p (const unsigned int&) The pass to apply
		*/
		void Apply(const std::string& tech, const unsigned int& p);

		/**
		* @brief Retrieves the number of passes of this effect by a given technique
		* @param[in] tech (const std::string&) The technique to check for
		* @return unsigned int The number of passes of this effect by technique
		*/
		unsigned int NumPasses(const std::string& tech);

		/**
		* @brief Converts a string to a sampling type
		* @param[in] str (const std::string&) The string to convert
		*/
		D3D11SamplerState::SamplerTypes StringToSampling(const std::string& str);

		/// Default destructor
		virtual ~D3D11Effect();

	private:
		std::map<std::string, Technique> techniques_; //!< The different techniques of this effect
	};
}