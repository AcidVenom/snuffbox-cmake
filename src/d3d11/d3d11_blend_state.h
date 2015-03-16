#pragma once

#include "../d3d11/d3d11_render_device.h"

namespace snuffbox
{
  /**
  * @class snuffbox::D3D11BlendState
  * @brief Used to set blend states per-object
  * @author Daniël Konings
  */
  class D3D11BlendState
  {
  public:
    /// Default constructor
    D3D11BlendState();

    /**
    * @brief Creates the actual blend state
    * @param[in] desc const D3D11_BLEND_DESC& The description of the blend state
    */
    void Create(const D3D11_BLEND_DESC& desc);

		/**
		* @brief Creates the blend state from a JSON
		* @param[in] json (const v8::Local<v8::Object>&) The JSON value
		*/
		void CreateFromJson(const v8::Local<v8::Object>& json);

    /**
    * @brief Creates the blend state from a JSON
    * @param[in] str (const std::string&) The JSON stringified
    */
    void CreateFromJson(const std::string& str);

    /// Sets this blend state
    void Set();

    /**
    * @brief Compares two blend state to see if a state change is needed
    * @param[in] other (snuffbox::D3D11BlendState*) The blend state to compare with
    * @return bool Are the descriptions equal?
    */
    bool EqualsTo(D3D11BlendState* other);

    /**
    * @return const D3D11_BLEND_DESC& The description of this blend state
    */
    const D3D11_BLEND_DESC& description() const;

    /// Default destructor
    ~D3D11BlendState();

    /**
    * @brief Converts a string to a blend enumerator
    * @param[in] str (const std::string&) The string to convert
    * @return D3D11_BLEND The converted value
    */
		static D3D11_BLEND StringToBlend(const std::string& str);

    /**
    * @brief Converts a string to a blend operation enumerator
    * @param[in] str (const std::string&) The string to convert
    * @return D3D11_BLEND_OP The converted value
    */
		static D3D11_BLEND_OP StringToBlendOp(const std::string& str);

  private:
    bool valid_; //!< Is this blend state valid?
    ID3D11BlendState* blend_state_; //!< The actual blend state
    D3D11_BLEND_DESC desc_; //!< The description of this blend state
  };
}