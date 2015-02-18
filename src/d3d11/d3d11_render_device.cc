#include "../d3d11/d3d11_render_device.h"
#include "../memory/shared_ptr.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	D3D11RenderDevice::D3D11RenderDevice()
	{

	}

	//---------------------------------------------------------------------------------------------------------
	D3D11RenderDevice* D3D11RenderDevice::Instance()
	{
		static SharedPtr<D3D11RenderDevice> render_device = AllocatedMemory::Instance().Construct<D3D11RenderDevice>();
		return render_device.get();
	}

	//---------------------------------------------------------------------------------------------------------
	bool D3D11RenderDevice::Initialise()
	{
		SNUFF_LOG_SUCCESS("Succesfully initialised the Direct3D 11 render device");
		return true;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderDevice::Draw()
	{

	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderDevice::Dispose()
	{
		SNUFF_LOG_INFO("Disposed the Direct3D 11 render device");
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11RenderDevice::~D3D11RenderDevice()
	{

	}
}