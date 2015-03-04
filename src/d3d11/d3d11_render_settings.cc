#include "../d3d11/d3d11_render_settings.h"
#include "../application/game.h"
#include "../platform/platform_window.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	D3D11RenderSettings::D3D11RenderSettings() :
		vsync_(false),
		resolution_(640.0f, 480.0f),
		invert_y_(false)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	D3D11RenderSettings* D3D11RenderSettings::Instance()
	{
		static SharedPtr<D3D11RenderSettings> settings = AllocatedMemory::Instance().Construct<D3D11RenderSettings>();
		return settings.get();
	}

	//---------------------------------------------------------------------------------------------------------
	const bool& D3D11RenderSettings::vsync() const
	{
		return vsync_;
	}

	//---------------------------------------------------------------------------------------------------------
	const XMFLOAT2& D3D11RenderSettings::resolution() const
	{
		return resolution_;
	}

	//---------------------------------------------------------------------------------------------------------
	const bool& D3D11RenderSettings::invert_y() const
	{
		return invert_y_;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderSettings::set_vsync(const bool& vsync)
	{
		vsync_ = vsync;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderSettings::set_resolution(const float& width, const float& height)
	{
		resolution_.x = width;
		resolution_.y = height;

    Window* window = Game::Instance()->window();
    D3D11RenderDevice::Instance()->ResizeBuffers(window->width(), window->height());
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderSettings::set_invert_y(const bool& value)
	{
		invert_y_ = value;
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11RenderSettings::~D3D11RenderSettings()
	{

	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderSettings::RegisterJS(JS_SINGLETON obj)
	{
		JSFunctionRegister funcs[] = {
			{ "setVsync", JSSetVsync },
			{ "vsync", JSVsync },
			{ "setResolution", JSSetResolution },
			{ "resolution", JSResolution },
			{ "setInvertY", JSSetInvertY },
			{ "invertY", JSInvertY }
		};

		JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderSettings::JSSetVsync(JS_ARGS args)
	{
		JSWrapper wrapper(args);

		if (wrapper.Check("B") == false)
		{
			SNUFF_LOG_WARNING("Unspecified value, defaulting to 'false'");
		}

		D3D11RenderSettings::Instance()->set_vsync(wrapper.GetValue<bool>(0, false));
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderSettings::JSVsync(JS_ARGS args)
	{
		JSWrapper wrapper(args);

		wrapper.ReturnValue<bool>(D3D11RenderSettings::Instance()->vsync());
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderSettings::JSSetResolution(JS_ARGS args)
	{
		JSWrapper wrapper(args);

		if (wrapper.Check("NN") == true)
		{
			D3D11RenderSettings::Instance()->set_resolution(
				wrapper.GetValue<float>(0, 640.0),
				wrapper.GetValue<float>(1, 480.0)
				);
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderSettings::JSResolution(JS_ARGS args)
	{
		JSWrapper wrapper(args);

		v8::Handle<v8::Object> obj = JSWrapper::CreateObject();

		const XMFLOAT2& resolution = D3D11RenderSettings::Instance()->resolution();
		JSWrapper::SetObjectValue<double>(obj, "w", resolution.x);
		JSWrapper::SetObjectValue<double>(obj, "h", resolution.y);

		wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderSettings::JSSetInvertY(JS_ARGS args)
	{
		JSWrapper wrapper(args);

		if (wrapper.Check("B") == false)
		{
			SNUFF_LOG_WARNING("Unspecified value, defaulting to 'false'");
		}

		D3D11RenderSettings::Instance()->set_invert_y(wrapper.GetValue<bool>(0, false));
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderSettings::JSInvertY(JS_ARGS args)
	{
		JSWrapper wrapper(args);

		wrapper.ReturnValue<bool>(D3D11RenderSettings::Instance()->invert_y());
	}
}