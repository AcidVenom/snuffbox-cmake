#pragma once

#include "../../d3d11/elements/d3d11_render_element.h"

namespace snuffbox
{
	class FBXModel;

  /**
  * @class snuffbox::D3D11Model
  * @brief A model element that uses an FBX model as vertex buffer
  * @author Daniël Konings
  */
  class D3D11Model : public D3D11RenderElement, public JSObject
  {
  public:
    /// Default constructor
		D3D11Model();

    /// JavaScript constructor
		D3D11Model(JS_ARGS args);

		/**
		* @brief Sets the model of this render element
		* @parma[in] path (const std::string&) The path to the model to set
		*/
		void SetModel(const std::string& path);

		/**
		* @return const std::string& The path of the model in use
		*/
		const std::string& path() const;

		/// @see snuffbox::D3D11RenderElement
		D3D11VertexBuffer* vertex_buffer();

    /// Default destructor
		virtual ~D3D11Model();

  private:
    FBXModel* model_; //!< The FBX model of this element
		std::string path_; //!< The path to the model this element was loaded with

  public:
    JS_NAME("Model");
    static void RegisterJS(JS_CONSTRUCTABLE obj);
		static void JSSetModel(JS_ARGS args);
		static void JSModel(JS_ARGS args);
  };
}