#pragma once

#include "../../d3d11/elements/d3d11_render_element.h"
#include <vector>

namespace snuffbox
{
  class MouseArea;

  /**
  * @class snuffbox::D3D11Widget
  * @brief A quad element which serves as a UI widget
  * @author Dani�l Konings
  */
	class D3D11Widget : public D3D11RenderElement, public JSObject
  {
  public:
    /// Default constructor
		D3D11Widget();

    /// JavaScript constructor
		D3D11Widget(JS_ARGS args);

    /// Creates the vertex buffer
    void Create();

    /**
    * @brief Adds a given mouse area from this widget
    * @param[in] area (snuffbox::MouseArea*) The mouse area to add
    */
    void AddMouseArea(MouseArea* area);

    /**
    * @brief Removes a given mouse area from this widget
    * @param[in] area (snuffbox::MouseArea*) The mouse area to remove
    */
    void RemoveMouseArea(MouseArea* area);

    /// @see snuffbox::D3D11RenderElement
    D3D11VertexBuffer* vertex_buffer();

		/// @see snuffbox::D3D11RenderElement
		const XMMATRIX& world_matrix();

    /// Default destructor
		virtual ~D3D11Widget();

  private:
    SharedPtr<D3D11VertexBuffer> vertex_buffer_; //!< The vertex buffer of this widget
		XMMATRIX world_matrix_; //!< The world matrix of this widget
    std::vector<MouseArea*> mouse_areas_; //!< The mouse areas assigned to this widget

  public:
    JS_NAME("Widget");
    static void RegisterJS(JS_CONSTRUCTABLE obj);
  };
}