#pragma once

#include "../d3d11/d3d11_vertex_buffer.h"
#include "../js/js_object.h"

namespace snuffbox
{
	/**
	* @class snuffbox::D3D11Line
	* @brief Provides auxillary utility to draw lines
	* @author Daniël Konings
	*/
	class D3D11Line : public JSObject
	{
	public:

		/// Default constructor
		D3D11Line();

		/**
		* @brief Retrieves the singleton instance of this class
		* @return snuffbox::D3D11Line* The pointer to the singleton instance
		*/
		static D3D11Line* Instance();

		/**
		* @brief Draws a line
		* @param[in] const float& (x1) The start X position
		* @param[in] const float& (y1) The start Y position
		* @param[in] const float& (z1) The start Z position
		* @param[in] const float& (r1) The start red value
		* @param[in] const float& (g1) The start green value
		* @param[in] const float& (b1) The start blue value
		* @param[in] const float& (x2) The end X position
		* @param[in] const float& (y2) The end Y position
		* @param[in] const float& (z2) The end Z position
		* @param[in] const float& (r2) The end red value
		* @param[in] const float& (g2) The end green value
		* @param[in] const float& (b2) The end blue value
		*/
		void DrawLine(
			const float& x1, const float& y1, const float& z1,
			const float& r1, const float& g1, const float& b1,
			const float& x2, const float& y2, const float& z2,
			const float& r2, const float& g2, const float& b2);

		/// Creates and draws the line buffer for 1 frame
		void Draw();

		/// Default destructor
		~D3D11Line();

	private:
		std::vector<Vertex> vertices_; //!< The current vertices to draw
		std::vector<int> indices_; //!< The current indices to draw
		SharedPtr<D3D11VertexBuffer> vertex_buffer_; //!< The vertex buffer of the line drawing
		SharedPtr<D3D11Material> material_; //!< The material used for line drawing
	public:
		JS_NAME("Line");
		static void RegisterJS(JS_SINGLETON obj);
		static void JSDraw(JS_ARGS args);
	};
}