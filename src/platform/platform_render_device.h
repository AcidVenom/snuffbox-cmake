#pragma once

#ifdef SNUFF_BUILD_OPENGL
#include "../ogl/ogl_render_device.h"
namespace snuffbox { typedef OGLRenderDevice PlatformRenderDevice; }
#else
#include "../d3d11/d3d11_render_device.h"
namespace snuffbox { typedef D3D11RenderDevice PlatformRenderDevice; }
#endif

namespace snuffbox
{
	class RenderDevice : public PlatformRenderDevice
	{
		/// Default constructor
		RenderDevice(){};

		/// Default destructor
		virtual ~RenderDevice(){};
	};
}