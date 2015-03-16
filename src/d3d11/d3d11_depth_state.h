#pragma once

#include "../d3d11/d3d11_render_device.h"

namespace snuffbox
{
	/**
	* @class snuffbox::D3D11DepthState
	* @brief Contains depth state information per-object
	* @author Daniël Konings
	*/
	class D3D11DepthState
	{
	public:
		/// Default constructor
		D3D11DepthState();

		/**
		* @brief Creates a depth state from a given description
		* @param[in] desc const D3D11_DEPTH_STENCIL_DESC& The description to create from
		*/
		void Create(const D3D11_DEPTH_STENCIL_DESC& desc);

		/**
		* @brief Creates the deoth state from a JSON
		* @param[in] json (const v8::Local<v8::Object>&) The JSON value
		*/
		void CreateFromJson(const v8::Local<v8::Object>& json);

    /**
    * @brief Creates the deoth state from a JSON
    * @param[in] str (std::string) The JSON stringified
    */
    void CreateFromJson(const std::string& str);

		/// Sets this depth state for use
		void Set();

    /**
    * @brief Compares this depth state to another depth state to see if it has to be changed
    * @param[in] state (snuffbox::D3D11DepthState*) The depth state to compare with
    * @return Is a state change required?
    */
    bool EqualsTo(D3D11DepthState* state);

    /**
    * @return const D3D11_DEPTH_STENCIL_DESC& The description of this depth state 
    */
    const D3D11_DEPTH_STENCIL_DESC& description() const;

		/// Default destructor
		~D3D11DepthState();

		/**
		* @brief Converts a string to a depth stencil operation
		* @param[in] str (const std::string&) The string to convert
		* @return D3D11_STENCIL_OP The converted value
		*/
		static D3D11_STENCIL_OP StringToOp(const std::string& str);

		/**
		* @brief Converts a string to a depth stencil write mask
		* @param[in] str (const std::string&) The string to convert
		* @return D3D11_DEPTH_WRITE_MASK The converted value
		*/
		static D3D11_DEPTH_WRITE_MASK StringToWriteMask(const std::string& str);

		/**
		* @brief Converts a string to a depth stencil comparison functino
		* @param[in] str (const std::string&) The string to convert
		* @return D3D11_COMPARISON_FUNC The converted value
		*/
		static D3D11_COMPARISON_FUNC StringToComparison(const std::string& str);

	private:
		bool valid_; //!< Is this depth state valid?
		ID3D11DepthStencilState* depth_state_; //!< The actual depth state
    D3D11_DEPTH_STENCIL_DESC desc_; //!< The description of this depth state
	};
}