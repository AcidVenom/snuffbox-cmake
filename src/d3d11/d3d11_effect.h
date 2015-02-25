#pragma once

#include "../d3d11/d3d11_render_device.h"
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
		* @param[in] path (std::string) The path to the JSON
		*/
		void Load(std::string path);

		/// Default destructor
		~D3D11Effect();

	private:
		bool valid_; //!< Is this effect valid?
		std::map<std::string, Technique> techniques_; //!< The different techniques of this effect
	};
}