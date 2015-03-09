#pragma once

#include "../js/js_object.h"
#include "../d3d11/d3d11_render_device.h"

namespace snuffbox
{
	/**
	* @class snuffbox::D3D11Camera
	* @brief A camera to handle projection logic
	* @author Daniël Konings
	*/
	class D3D11Camera : public JSObject
	{
	public:

		/**
		* @enum snuffbox::D3D11Camera::CameraTypes
		* @brief Contains enumerators for the different camera types
		* @author Daniël Konings
		*/
		enum CameraTypes
		{
			kPerspective,
			kOrthographic
		};

		/**
		* @enum snuffbox::D3D11Camera::CameraTransformation
		* @brief Different types of camera transformation
		* @author Daniël Konings
		*/
		enum CameraTransformation
		{
			kLocal,
			kAbsolute
		};

	public:
		/**
		* @brief Constructs a camera by type
		* @param[in] type const snuffbox::D3D11Camera::CameraTypes& The type to construct the camera with
		*/ 
		D3D11Camera(const D3D11Camera::CameraTypes& type);

		/// JavaScript constructor
		D3D11Camera(JS_ARGS args);

		/**
		* @return const XMVECTOR& The translation of the camera
		*/
		const XMVECTOR& translation() const;

		/**
		* @return const XMVECTOR& The rotation of the camera
		*/
		const XMVECTOR& rotation() const;

		/**
		* @return const XMVECTOR& The 'up' vector of the camera
		*/
		const XMVECTOR& up() const;

		/**
		* @brief Calculates the projection matrix and returns it, based on type
		* @return const XMMATRIX& The projection matrix
		*/
		const XMMATRIX& projection();

		/**
		* @brief Calculates the view matrix and returns it
		* @return const XMMATRIX& The view matrix
		*/
		const XMMATRIX& view();

		/**
		* @brief Calculates the view vector and returns it
		* @return XMFLOAT3 The view vector of the camera
		*/
		XMFLOAT3 view_vector();

		/**
		* @return const float& The near plane of the camera
		*/
		const float& near_plane() const;

		/**
		* @return const float& The far plane of the camera
		*/
		const float& far_plane() const;

		/**
		* @return const float& The field of view of the camera
		*/
		const float& fov() const;

		/**
		* @return const snuffbox::D3D11Camera::CameraTypes& The type of the camera
		*/
		const CameraTypes& type() const;

		/**
		* @brief Sets the translation of the camera
		* @param[in] x (const float&) The new x position
		* @param[in] y (const float&) The new y position
		* @param[in] z (const float&) The new z position
		*/
		void set_translation(const float& x, const float& y, const float& z);

		/**
		* @brief Sets the rotation of the camera
		* @param[in] x (const float&) The new x rotation
		* @param[in] y (const float&) The new y rotation
		* @param[in] z (const float&) The new z rotation
		*/
		void set_rotation(const float& x, const float& y, const float& z);

		/**
		* @brief Sets the near plane of the camera
		* @param[in] plane (const float&) The new near plane
		*/
		void set_near_plane(const float& plane);

		/**
		* @brief Sets the far plane of the camera
		* @param[in] plane (const float&) The new far plane
		*/
		void set_far_plane(const float& plane);

		/**
		* @brief Sets the field of view of the camera
		* @param[in] fov (const float&) The new field of view
		*/
		void set_fov(const float& fov);

		/**
		* @brief Translate by given values
		* @param[in] x (const float&) The translation on the x-axis to add
		* @param[in] y (const float&) The translation on the y-axis to add
		* @param[in] z (const float&) The translation on the z-axis to add
		* @param[in] trans (const snuffbox::Camera::CameraTransformation&) The transformation type
		*/
		void TranslateBy(const float& x, const float& y, const float& z, const CameraTransformation& trans);

		/**
		* @brief Rotate by given values
		* @param[in] x (const float&) The rotation on the x-axis to add
		* @param[in] y (const float&) The rotation on the y-axis to add
		* @param[in] z (const float&) The rotation on the z-axis to add
		*/
		void RotateBy(const float& x, const float& y, const float& z);

		/**
		* @brief Unprojects 2 points onto a camera plane
		* @param[in] px (const float&) The x position
		* @param[in] py (const float&) The y position
		* @param[in] plane (const float&) The plane to unproject on
		* @return XMFLOAT3 The unprojected coordinates in 3D space
		*/
		XMFLOAT3 Unproject(const float& px, const float& py, const float& plane);

		/// Default destructor
		virtual ~D3D11Camera();

	private:
		CameraTypes type_; //!< The type of the camera
		XMVECTOR translation_; //!< The translation of the camera
		XMVECTOR rotation_; //!< The rotation of the camera
		XMVECTOR up_; //!< The 'up' vector of the camera
		XMVECTOR right_; //!< The right vector of the camera
		XMVECTOR forward_; //!< The forward vector of the camera
		XMVECTOR target_; //!< The target vector of the camera
		float move_left_right_; //!< The movement left/right
		float move_back_forward_; //!< The movement back/forward
		float move_up_down_; //!< The movement up/down
		XMMATRIX view_; //!< The view matrix
		XMMATRIX projection_; //!< The projection matrix
		float near_plane_; //!< The camera near plane
		float far_plane_; //!< The camera far plane
		float fov_; //!< The field of view of the camera
		const XMVECTOR cam_forward_ = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		const XMVECTOR cam_right_ = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	public:
		JS_NAME("Camera");
		static void Enumerate();
		static void RegisterJS(JS_CONSTRUCTABLE obj);
		static void JSSetTranslation(JS_ARGS args);
		static void JSTranslateBy(JS_ARGS args);
		static void JSTranslation(JS_ARGS args);
		static void JSSetRotation(JS_ARGS args);
		static void JSRotateBy(JS_ARGS args);
		static void JSRotation(JS_ARGS args);
		static void JSSetNearPlane(JS_ARGS args);
		static void JSNearPlane(JS_ARGS args);
		static void JSSetFarPlane(JS_ARGS args);
		static void JSFarPlane(JS_ARGS args);
		static void JSSetFov(JS_ARGS args);
		static void JSFov(JS_ARGS args);
		static void JSUnproject(JS_ARGS args);
	};
}