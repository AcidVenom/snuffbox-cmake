#pragma once

#include "../../js/js_object.h"
#include "../../d3d11/d3d11_render_device.h"

namespace snuffbox
{
  class D3D11VertexBuffer;
	class D3D11Material;

  /**
  * @class snuffbox::D3D11RenderElement
  * @brief Contains information for the render device to use in drawing different elements
  * @author Daniël Konings
  */
  class D3D11RenderElement
  {
	public:

		/**
		* @enum snuffbox::D3D11RenderElement::LayerType
		* @brief Contains enumerators to specify the layer an element should be spawned on
		* @author Daniël Konings
		*/
		enum LayerType
		{
			kWorld,
			kUI
		};

  public:
    /// Default constructor
    D3D11RenderElement();

		/// JavaScript constructor
		D3D11RenderElement(JS_ARGS args);

    /// Default destructor
    ~D3D11RenderElement();

    /**
    * @brief Spawns this element on a given render target by name
    * @param[in] target (const std::string&) The name of the target to spawn the element on
    */
    void Spawn(const std::string& target);

    /// Destroys the render element
    void Destroy();

    /**
    * @brief Translate this render element by given values
    * @param[in] x (const float&) The translation on the x-axis to add
    * @param[in] y (const float&) The translation on the y-axis to add
    * @param[in] z (const float&) The translation on the z-axis to add
    */
		void TranslateBy(const float& x, const float& y, const float& z);

    /**
    * @brief Rotate this render element by given values
    * @param[in] x (const float&) The rotation on the x-axis to add
    * @param[in] y (const float&) The rotation on the y-axis to add
    * @param[in] z (const float&) The rotation on the z-axis to add
    */
		void RotateBy(const float& x, const float& y, const float& z);

		/**
		* @brief Calculates the world matrix and stores it
		* @param[out] world (XMMATRIX*) The matrix to store the world matrix in
		* @param[in] invert_y (const bool&) Should the Y axis be inverted? Default = false
		*/
		void CalculateWorldMatrix(XMMATRIX* world, const bool& invert_y = false);

    /**
    * @return snuffbox::D3D11VertexBuffer* The vertex buffer associated with this render element
    */
    virtual D3D11VertexBuffer* vertex_buffer() = 0;

    /**
    * @return const XMVECTOR& The translation of this render element
    */
    const XMVECTOR& translation() const;

    /**
    * @return const XMVECTOR& The rotation of this render element
    */
    const XMVECTOR& rotation() const;

    /**
    * @return const XMVECTOR& The scale of this render element
    */
    const XMVECTOR& scale() const;

    /**
    * @return const XMVECTOR& The offet of this render element
    */
    const XMVECTOR& offset() const;

    /**
    * @return const XMVECTOR& The size of this render element
    */
    const XMVECTOR& size() const;

    /**
    * @return const XMMATRIX& The world matrix of this render element
    */
    virtual const XMMATRIX& world_matrix();

    /**
    * @return const bool& Was this render element spawned?
    */
    const bool& spawned() const;

		/**
		* @return snuffbox::D3D11Material* The material of this render element
		*/
		D3D11Material* material();

		/**
		* @return const std::string& The technique of this render element
		*/
		const std::string& technique() const;

		/**
		* @return snuffbox::D3D11RenderElement* The parent of this render element
		*/
		D3D11RenderElement* parent();

		/**
		* @return  constsnuffbox::D3D11RenderElement::LayerType& The layer type of this render element
		*/
		const LayerType& layer_type() const;

    /**
    * @brief Sets the translation of this render element
    * @param[in] x (const float&) The new x position
    * @param[in] y (const float&) The new y position
    * @param[in] z (const float&) The new z position
    */
		void set_translation(const float& x, const float& y, const float& z);

    /**
    * @brief Sets the rotation of this render element
    * @param[in] x (const float&) The new x rotation
    * @param[in] y (const float&) The new y rotation
    * @param[in] z (const float&) The new z rotation
    */
		void set_rotation(const float& x, const float& y, const float& z);

    /**
    * @brief Sets the scaling of this render element
    * @param[in] x (const float&) The new x scaling
    * @param[in] y (const float&) The new y scaling
    * @param[in] z (const float&) The new z scaling
    */
		void set_scale(const float& x, const float& y, const float& z);

    /**
    * @brief Sets the offset of this render element
    * @param[in] x (const float&) The new x offset
    * @param[in] y (const float&) The new y offset
    * @param[in] z (const float&) The new z offset
    */
		void set_offset(const float& x, const float& y, const float& z);

    /**
    * @brief Sets the sizing of this render element
    * @param[in] x (const float&) The new x sizing
    * @param[in] y (const float&) The new y sizing
    * @param[in] z (const float&) The new z sizing
    */
		void set_size(const float& x, const float& y, const float& z);

		/**
		* @brief Sets the material of this render element
		* @param[in] path (const std::string&) The path to the effect file
		*/
		void set_material(const std::string& path);

		/**
		* @brief Sets the material of this render element
		* @param[in] path (D3D11Material*) The pointer to the effect
		*/
		void set_material(D3D11Material* material);

		/**
		* @brief Sets the technique of this render element
		* @param[in] technique (std::string) The name of the technique
		*/
		void set_technique(const std::string& technique);

		/**
		* @brief Sets the parent of this render element
		* @param[in] parent (D3D11RenderElement*) The parent to this element
		*/
		void set_parent(D3D11RenderElement* parent);

		/**
		* @brief Sets the layer type of this render element
		* @param[in] type (const snuffbox::D3D11RenderElement::LayerType&) The type to set
		*/
		void set_layer_type(const LayerType& type);

		/**
		* @brief Sets the world matrix of this render element
		* @param[in] matrix (const XMMATRIX&) The matrix to set
		*/
		void set_world_matrix(const XMMATRIX& matrix);

		/**
		* @brief Sets if this render element should billboard towards the camera
		* @param[in] billboarding (const bool&) The boolean value
		*/
		void set_billboarding(const bool& billboarding);

  private:
    XMVECTOR translation_; //!< The translation vector of this render element
    XMVECTOR rotation_; //!< The rotation vector of this render element
    XMVECTOR scale_; //!< The scaling vector of this render element
    XMVECTOR offset_; //!< The offset vector of this render element
    XMVECTOR size_; //!< The size vector of this render element
    XMMATRIX world_matrix_; //!< The world matrix of this render element
    bool spawned_; //!< Is this render element spawned?
		D3D11Material* material_; //!< The effect of this render element
		std::string technique_; //!< The technique of this render element
		D3D11RenderElement* parent_; //!< The parent of this render element
		LayerType layer_type_; //!< The layer type of this render element
		bool billboarding_; //!< Should this render element billboard towards the camera?

  public:
    static void Register(JS_CONSTRUCTABLE obj);
    static void JSSetTranslation(JS_ARGS args);
    static void JSTranslateBy(JS_ARGS args);
    static void JSTranslation(JS_ARGS args);
    static void JSSetRotation(JS_ARGS args);
    static void JSRotateBy(JS_ARGS args);
    static void JSRotation(JS_ARGS args);
    static void JSSetScale(JS_ARGS args);
    static void JSScale(JS_ARGS args);
    static void JSSetOffset(JS_ARGS args);
    static void JSOffset(JS_ARGS args);
    static void JSSetSize(JS_ARGS args);
    static void JSSize(JS_ARGS args);
		static void JSSetMaterial(JS_ARGS args);
		static void JSSetTechnique(JS_ARGS args);
		static void JSSetParent(JS_ARGS args);
    static void JSSpawn(JS_ARGS args);
		static void JSSpawned(JS_ARGS args);
    static void JSDestroy(JS_ARGS args);
  };
}