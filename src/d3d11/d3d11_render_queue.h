#pragma once

#include "../d3d11/d3d11_render_device.h"
#include <vector>

namespace snuffbox
{
  class D3D11RenderElement;

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
    /// Default constructor
    D3D11RenderQueue();

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

    /// Clears the entire render queue
    void Clear();

  private:
    std::vector<D3D11RenderElement*> world_; //!< A list of world elements to sort and / or draw
		std::vector<D3D11RenderElement*> ui_; //!< A list of UI elements to sort and / or draw
  };
}