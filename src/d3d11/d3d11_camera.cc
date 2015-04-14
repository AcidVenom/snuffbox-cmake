#pragma once

#include "../d3d11/d3d11_camera.h"
#include "../d3d11/d3d11_render_settings.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	D3D11Camera::D3D11Camera(const D3D11Camera::CameraTypes& type) :
		translation_(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)),
		rotation_(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)),
		up_(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)),
		near_plane_(0.01f),
		far_plane_(1000.0f),
		type_(type),
		fov_(XM_PI / 2),
		right_(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f)),
		forward_(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)),
		target_(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)),
		move_left_right_(0.0f),
		move_back_forward_(0.0f),
		move_up_down_(0.0f),
		zoom_(1.0f)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	D3D11Camera::D3D11Camera(JS_ARGS args) :
		translation_(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)),
		rotation_(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)),
		up_(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f)),
		near_plane_(0.01f),
		far_plane_(1000.0f),
		fov_(XM_PI / 2),
		right_(XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f)),
		forward_(XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f)),
		target_(XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f)),
		move_left_right_(0.0f),
		move_back_forward_(0.0f),
		move_up_down_(0.0f),
		zoom_(1.0f)
	{
		JSWrapper wrapper(args);

		type_ = static_cast<CameraTypes>(wrapper.GetValue<int>(0, 0));
	}

	//---------------------------------------------------------------------------------------------------------
	const XMVECTOR& D3D11Camera::translation() const
	{
		return translation_;
	}

	//---------------------------------------------------------------------------------------------------------
	const XMVECTOR& D3D11Camera::rotation() const
	{
		return rotation_;
	}

	//---------------------------------------------------------------------------------------------------------
	const XMVECTOR& D3D11Camera::up() const
	{
		return up_;
	}

	//---------------------------------------------------------------------------------------------------------
	const XMMATRIX& D3D11Camera::projection()
	{
		const XMFLOAT2& res = D3D11RenderSettings::Instance()->resolution();
		if (type_ == CameraTypes::kOrthographic)
		{
			projection_ = XMMatrixOrthographicRH(res.x / zoom_, res.y / zoom_, near_plane_, far_plane_);
		}
		else if (type_ == CameraTypes::kPerspective)
		{
			projection_ = XMMatrixPerspectiveFovRH(fov_, res.x / res.y, near_plane_, far_plane_);
		}

		return projection_;
	}

	//---------------------------------------------------------------------------------------------------------
	const XMMATRIX& D3D11Camera::view()
	{
		XMMATRIX rot = XMMatrixRotationRollPitchYawFromVector(rotation_);
		target_ = XMVector3TransformCoord(cam_forward_, rot);
		target_ = XMVector3Normalize(target_);

		right_ = XMVector3TransformCoord(cam_right_, rot);
		forward_ = XMVector3TransformCoord(cam_forward_, rot);
		up_ = XMVector3Cross(forward_, right_);

		translation_ += move_left_right_ * right_;
		translation_ += move_back_forward_ * forward_;
		translation_ += move_up_down_ * up_;

		target_ = translation_ + target_;

		view_ = XMMatrixLookAtLH(translation_, target_, up_);
		move_left_right_ = 0.0f;
		move_back_forward_ = 0.0f;
		move_up_down_ = 0.0f;

		return view_;
	}

	//---------------------------------------------------------------------------------------------------------
	const float& D3D11Camera::near_plane() const
	{
		return near_plane_;
	}

	//---------------------------------------------------------------------------------------------------------
	const float& D3D11Camera::far_plane() const
	{
		return far_plane_;
	}

	//---------------------------------------------------------------------------------------------------------
	const float& D3D11Camera::fov() const
	{
		return fov_;
	}

	//---------------------------------------------------------------------------------------------------------
	const D3D11Camera::CameraTypes& D3D11Camera::type() const
	{
		return type_;
	}

	//---------------------------------------------------------------------------------------------------------
	const float& D3D11Camera::zoom() const
	{
		return zoom_;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::set_translation(const float& x, const float& y, const float& z)
	{
		float yy = D3D11RenderSettings::Instance()->invert_y() == false ? -y : y;
		translation_ = XMVectorSet(x, yy, z, 1.0f);
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::set_rotation(const float& x, const float& y, const float& z)
	{
		rotation_ = XMVectorSet(x, y, z, 1.0f);
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::set_near_plane(const float& plane)
	{
		near_plane_ = plane;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::set_far_plane(const float& plane)
	{
		far_plane_ = plane;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::set_fov(const float& fov)
	{
		fov_ = fov;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::set_zoom(const float& zoom)
	{
		zoom_ = zoom;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::TranslateBy(const float& x, const float& y, const float& z, const CameraTransformation& trans)
	{
		switch (trans)
		{
		case CameraTransformation::kLocal:
			move_left_right_ += x;
			move_back_forward_ += z;
			move_up_down_ += y;
			break;

		case CameraTransformation::kAbsolute:
			translation_ += XMVectorSet(x, y, z, 1.0f);
			break;
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::RotateBy(const float& x, const float& y, const float& z)
	{
		rotation_ += XMVectorSet(x, y, z, 0.0f);
	}

	//---------------------------------------------------------------------------------------------------------
	XMFLOAT3 D3D11Camera::Unproject(const float& px, const float& py, const float& pz)
	{
		const XMFLOAT2& res = D3D11RenderSettings::Instance()->resolution();

		XMVECTOR coords = XMVector3Unproject(XMVectorSet(px, res.y - py, pz, 1.0f), 0.0f, 0.0f, res.x, res.y, near_plane_, far_plane_, projection_, view_, XMMatrixIdentity());

		XMFLOAT3 to_ret;
		XMStoreFloat3(&to_ret, coords);

		to_ret.y *= D3D11RenderSettings::Instance()->invert_y() == false ? -1.0f : 1.0f;

		return to_ret;
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11Camera::~D3D11Camera()
	{

	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::Enumerate()
	{
		v8::Handle<v8::Object> obj = JSWrapper::CreateObject();

		JSWrapper::SetObjectValue<double>(obj, "Perspective", 0.0);
		JSWrapper::SetObjectValue<double>(obj, "Orthographic", 1.0);

		JSStateWrapper::Instance()->RegisterGlobal("CameraType", obj);

    obj = JSWrapper::CreateObject();

    JSWrapper::SetObjectValue<double>(obj, "Local", 0.0);
    JSWrapper::SetObjectValue<double>(obj, "Absolute", 1.0);

    JSStateWrapper::Instance()->RegisterGlobal("CameraTransformation", obj);
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::RegisterJS(JS_CONSTRUCTABLE obj)
	{
		D3D11Camera::Enumerate();

		JSFunctionRegister funcs[] = {
			{ "setTranslation", JSSetTranslation },
			{ "translateBy", JSTranslateBy },
			{ "translation", JSTranslation },
			{ "setRotation", JSSetRotation },
			{ "rotateBy", JSRotateBy },
			{ "rotation", JSRotation },
			{ "setNearPlane", JSSetNearPlane },
			{ "nearPlane", JSNearPlane },
			{ "setFarPlane", JSSetFarPlane },
			{ "farPlane", JSFarPlane },
			{ "setFov", JSSetFov },
			{ "fov", JSFov },
			{ "unproject", JSUnproject },
			{ "setZoom", JSSetZoom },
      { "zoom", JSZoom }
		};

		JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::JSSetTranslation(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Camera* self = wrapper.GetPointer<D3D11Camera>(args.This());

		if (wrapper.Check("NNN") == true)
		{
			self->set_translation(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f), wrapper.GetValue<float>(2, 0.0f));
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::JSTranslateBy(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Camera* self = wrapper.GetPointer<D3D11Camera>(args.This());

		if (wrapper.Check("NNN") == true)
		{
			self->TranslateBy(
				wrapper.GetValue<float>(0, 0.0f), 
				wrapper.GetValue<float>(1, 0.0f), 
				wrapper.GetValue<float>(2, 0.0f), 
				static_cast<CameraTransformation>(wrapper.GetValue<int>(3, 0)));
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::JSTranslation(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Camera* self = wrapper.GetPointer<D3D11Camera>(args.This());

		v8::Handle<v8::Object> obj = JSWrapper::CreateObject();
		const XMVECTOR& v = self->translation();

		float x = XMVectorGetX(v);
		float y = XMVectorGetY(v);
		float z = XMVectorGetZ(v);

		float yy = D3D11RenderSettings::Instance()->invert_y() == false ? -y : y;
		JSWrapper::SetObjectValue(obj, "x", x);
		JSWrapper::SetObjectValue(obj, "y", yy);
		JSWrapper::SetObjectValue(obj, "z", z);

		wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::JSSetRotation(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Camera* self = wrapper.GetPointer<D3D11Camera>(args.This());

		if (wrapper.Check("NNN") == true)
		{
			self->set_rotation(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f), wrapper.GetValue<float>(2, 0.0f));
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::JSRotateBy(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Camera* self = wrapper.GetPointer<D3D11Camera>(args.This());

		if (wrapper.Check("NNN") == true)
		{
			self->RotateBy(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f), wrapper.GetValue<float>(2, 0.0f));
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::JSRotation(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Camera* self = wrapper.GetPointer<D3D11Camera>(args.This());

		v8::Handle<v8::Object> obj = JSWrapper::CreateObject();
		const XMVECTOR& v = self->rotation();

		float x = XMVectorGetX(v);
		float y = XMVectorGetY(v);
		float z = XMVectorGetZ(v);

		JSWrapper::SetObjectValue(obj, "x", x);
		JSWrapper::SetObjectValue(obj, "y", y);
		JSWrapper::SetObjectValue(obj, "z", z);

		wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::JSSetNearPlane(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Camera* self = wrapper.GetPointer<D3D11Camera>(args.This());

		wrapper.Check("N");
		self->set_near_plane(wrapper.GetValue<float>(0, 1.0f));
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::JSNearPlane(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Camera* self = wrapper.GetPointer<D3D11Camera>(args.This());

		wrapper.ReturnValue<float>(self->near_plane());
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::JSSetFarPlane(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Camera* self = wrapper.GetPointer<D3D11Camera>(args.This());

		wrapper.Check("N");
		self->set_far_plane(wrapper.GetValue<float>(0, 1000.0f));
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::JSFarPlane(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Camera* self = wrapper.GetPointer<D3D11Camera>(args.This());

		wrapper.ReturnValue<float>(self->far_plane());
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::JSSetFov(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Camera* self = wrapper.GetPointer<D3D11Camera>(args.This());

		wrapper.Check("N");
		self->set_fov(wrapper.GetValue<float>(0, XM_PI * 2));
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::JSFov(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Camera* self = wrapper.GetPointer<D3D11Camera>(args.This());

		wrapper.ReturnValue<float>(self->fov());
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::JSUnproject(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Camera* self = wrapper.GetPointer<D3D11Camera>(args.This());

		if (wrapper.Check("NNN") == true)
		{
			const XMFLOAT3& p = self->Unproject(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f), wrapper.GetValue<float>(2, 0.0f));
			
			v8::Handle<v8::Object> obj = JSWrapper::CreateObject();
			JSWrapper::SetObjectValue(obj, "x", p.x);
			JSWrapper::SetObjectValue(obj, "y", p.y);
			JSWrapper::SetObjectValue(obj, "z", p.z);

			wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::JSSetZoom(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Camera* self = wrapper.GetPointer<D3D11Camera>(args.This());

		if (wrapper.Check("N") == true)
		{
			self->set_zoom(wrapper.GetValue<float>(0, 1.0f));
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Camera::JSZoom(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Camera* self = wrapper.GetPointer<D3D11Camera>(args.This());

		wrapper.ReturnValue<double>(self->zoom());
	}
}