#-------------------------------------------------------------------
# This file was taken from the CMake build system for OGRE
#     (Object-oriented Graphics Rendering Engine)
# For the latest info, see http://www.ogre3d.org/
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Find DirectX SDK
# Define:
# DirectX_FOUND
# DirectX_INCLUDE_DIR
# DirectX_OLD_INCLUDE_DIR
# DirectX_LIBRARY
# DirectX_LIBRARY_DIR
# DirectX_ROOT_DIR

if(WIN32)
  include(FindPkgMacros)
  findpkg_begin(DirectX)
  
  getenv_path(DXSDK_DIR)
  getenv_path(DIRECTX_HOME)
  getenv_path(DIRECTX_ROOT)
  getenv_path(DIRECTX_BASE)

  set(DirectX_PREFIX_PATH 
    "${DXSDK_DIR}" "${ENV_DXSDK_DIR}"
    "${DIRECTX_HOME}" "${ENV_DIRECTX_HOME}"
    "${DIRECTX_ROOT}" "${ENV_DIRECTX_ROOT}"
    "${DIRECTX_BASE}" "${ENV_DIRECTX_BASE}"
    "C:/apps_x86/Microsoft DirectX SDK*"
    "C:/Program Files (x86)/Microsoft DirectX SDK*"
    "C:/apps/Microsoft DirectX SDK*"
    "C:/Program Files/Microsoft DirectX SDK*"
    "$ENV{ProgramFiles}/Microsoft DirectX SDK*"
  )

  set(DirectX_INC_SEARCH_PATH 
    "C:/Program Files (x86)/Windows Kits/8.1/Include/shared"
    "C:/Program Files (x86)/Windows Kits/8.1/Include/um"
    "C:/Program Files (x86)/Windows Kits/8.0/Include/shared"
    "C:/Program Files (x86)/Windows Kits/8.0/Include/um"
  )
  set(DirectX_LIB_SEARCH_PATH 
    "C:/Program Files (x86)/Windows Kits/8.1/Lib/winv6.3/um"
    "C:/Program Files (x86)/Windows Kits/8.0/Lib/win8/um"
  )

  set(DirectX_BIN_SEARCH_PATH 
    "C:/Program Files (x86)/Windows Kits/8.1/bin"
    "C:/Program Files (x86)/Windows Kits/8.0/bin"
  )

  create_search_paths(DirectX)
  clear_if_changed(DirectX_PREFIX_PATH
    DirectX_LIBRARY
  DirectX_INCLUDE_DIR
  )
  
  find_path(DirectX_OLD_INCLUDE_DIR NAMES d3dx10.h HINTS ${DirectX_INC_SEARCH_PATH})
  find_path(DirectX_INCLUDE_DIR NAMES d3d9.h HINTS ${DirectX_INC_SEARCH_PATH})
  if(CMAKE_CL_64)
    set(DirectX_LIBPATH_SUFFIX "x64")
  else(CMAKE_CL_64)
    set(DirectX_LIBPATH_SUFFIX "x86")
  endif(CMAKE_CL_64)
  find_library(DirectX_LIBRARY NAMES d3d11 HINTS ${DirectX_LIB_SEARCH_PATH} PATH_SUFFIXES ${DirectX_LIBPATH_SUFFIX})

  findpkg_finish(DirectX)
  set(DirectX_LIBRARIES ${DirectX_LIBRARIES})
  

  if (DirectX_FOUND)
    find_path(DirectX_D3D11_INCLUDE_DIR NAMES D3D11Shader.h HINTS ${DirectX_INC_SEARCH_PATH})
    get_filename_component(DirectX_LIBRARY_DIR "${DirectX_LIBRARY}" PATH)
    find_library(DirectX_D3D11_LIBRARY NAMES d3d11 HINTS ${DirectX_LIB_SEARCH_PATH} PATH_SUFFIXES ${DirectX_LIBPATH_SUFFIX})
    find_library(DirectX_D3DX10_LIBRARY NAMES d3dx10 HINTS ${DirectX_LIB_SEARCH_PATH} PATH_SUFFIXES ${DirectX_LIBPATH_SUFFIX})
    find_library(DirectX_D3DX11_LIBRARY NAMES d3dx11 HINTS ${DirectX_LIB_SEARCH_PATH} PATH_SUFFIXES ${DirectX_LIBPATH_SUFFIX})
  if (DirectX_D3D11_INCLUDE_DIR AND DirectX_D3D11_LIBRARY)
    set(DirectX_D3D11_FOUND TRUE)
    set(DirectX_D3D11_INCLUDE_DIR ${DirectX_D3D11_INCLUDE_DIR})
    set(DirectX_D3D11_LIBRARIES ${DirectX_D3D11_LIBRARIES}
      ${DirectX_D3D11_LIBRARY}
      ${DirectX_D3DX10_LIBRARY}
      ${DirectX_D3DX11_LIBRARY}
      ${DirectX_DXGI_LIBRARY}        
      ) 
    endif ()
	set (DirectX_D3D11_LIBRARIES "${DirectX_D3D11_LIBRARIES};Ws2_32.lib")
  endif ()
else()
	message (FATAL_ERROR "Attempting to load DirectX 11 on a non-directx platform!")
endif(WIN32)