#pragma once

#include "../d3d11/d3d11_render_device.h"
#include "../d3d11/elements/d3d11_render_element.h"
#include <vector>

namespace snuffbox
{
	struct RenderSorterZ
	{
		bool operator()(D3D11RenderElement* a, D3D11RenderElement* b);
	};

	struct RenderSorterDistance
	{
		bool operator()(D3D11RenderElement* a, D3D11RenderElement* b);
	};

  /**
  * @class snuffbox::D3D11RenderQueue
  * @brief Handles all sorting of render elements, also used to draw elements in the queue
  * @author Daniël Konings
  */
  class D3D11RenderQueue
  {
  public:
    /**
    * @enum snuffbox::D3D11RenderQueue::SortMethods
    * @brief Contains enumerators to specify the sorting methods
    * @author Daniël Konings
    */
    enum SortMethods
    {
      kZSorting,
      kDistanceFromCamera
    };

  public:
    /**
		* @brief Constructs this queue with a given render target
		* @param[in] target (snuffbox::D3D11RenderTarget*) The target parent of this queue
		*/
    D3D11RenderQueue(D3D11RenderTarget* target);

    /// Default destructor
    ~D3D11RenderQueue();

    /**
    * @brief Adds a render element to this render queue
    * @param[in] element (snuffbox::D3D11RenderElement*) The element to add
    */
    void Add(D3D11RenderElement* element);

    /**
    * @brief Sorts the render elements
    * @param[in] method (const snuffbox::D3D11RenderQueue::SortMethods&) The method to sort with
    */
    void Sort(const SortMethods& method);

    /**
    * @brief Draws an element to the current render target
    * @param[in] element (snuffbox::D3D11RenderElement*) The element to draw
    * @param[in] context (ID3D11DeviceContext*) The context to draw with
    */
    void DrawElement(ID3D11DeviceContext* context, D3D11RenderElement* element);

    /**
    * @brief Draws the entire queue to the current render target
    * @param[in] context (ID3D11DeviceContext*) The context to draw with
    */
    void Draw(ID3D11DeviceContext* context);

    /**
    * @brief Renders the buffer with the given materials and given offset
    * @param[in] m_group (snuffbox::D3D11RenderElement::MaterialGroup&) The material group to render
    * @param[in] buffer (snuffbox::D3D11VertexBuffer*) The vertex buffer to render
    * @param[in] element (snuffbox::D3D11RenderElement*) The element currently being drawn
    * @param[in] start (const int&) The start index
    * @param[in] end (const int&) The end index
    */
    void Render(D3D11RenderElement::MaterialGroup& m_group, D3D11VertexBuffer* buffer, D3D11RenderElement* element, const int& start = 0, const int& end = -1);

    /// Clears the entire render queue
    void Clear();

    /**
    * @brief Removes a render element from the queue
    * @param[in] ptr (snuffbox::D3D11RenderElement*) The element to remove
    */
    void Remove(D3D11RenderElement* ptr);

    /**
    * @brief Sets the render target of this queue
    * @param[in] target (snuffbox::D3D11RenderTarget*) The render target to set
    */
    void set_target(D3D11RenderTarget* target);

  private:
    std::vector<D3D11RenderElement*> world_; //!< A list of world elements to sort and / or draw
		std::vector<D3D11RenderElement*> ui_; //!< A list of UI elements to sort and / or draw
		D3D11RenderTarget* target_; //!< The owner of this render queue
  };
}