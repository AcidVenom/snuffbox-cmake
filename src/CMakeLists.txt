IF (SNUFF_BUILD_CONSOLE)
	SET (CMAKE_AUTOMOC ON)
	SET (CMAKE_AUTOUIC ON)
	
	SET (CMAKE_INCLUDE_CURRENT_DIR ON)
ENDIF (SNUFF_BUILD_CONSOLE)

SET (Win32Sources 
	win32/win32_window.cc
	win32/win32_window.h
	win32/win32_file_watch.cc
	win32/win32_file_watch.h
	win32/win32_text_file.h
	win32/win32_text_file.cc
)

SET (OSXSources 
	osx/osx_window.cc
	osx/osx_window.h
	osx/osx_file_watch.cc
	osx/osx_file_watch.h
	osx/osx_text_file.h
	osx/osx_text_file.cc
)

SET (LinuxSources 
	linux/linux_window.cc
	linux/linux_window.h
	linux/linux_file_watch.cc
	linux/linux_file_watch.h
	linux/linux_text_file.h
	linux/linux_text_file.cc
)

SET (PlatformSources
	platform/platform_window_base.h
	platform/platform_window.h
	platform/platform_file_watch_base.h
	platform/platform_file_watch.h
	platform/platform_text_file_base.h
	platform/platform_text_file.h
)

SET (ApplicationSources
	application/main.cc
	application/game.h
	application/game.cc
	application/logging.cc
	application/logging.h
)

SET (D3DSources
	d3d11/d3d11_render_device.h
	d3d11/d3d11_render_device.cc
)

SET (OGLSources
	ogl/ogl_render_device.h
	ogl/ogl_render_device.cc
)

SET (MemorySources
	memory/allocated_memory.h
	memory/allocated_memory.cc
	memory/shared_ptr.h
)

SET (InputSources
	input/key.h
	input/key.cc
	input/keyboard.h
	input/keyboard.cc
	input/mouse.h
	input/mouse.cc
)

SET (JavaScriptSources
	js/js_object.h
	js/js_callback.h
	js/js_function_register.h
	js/js_function_register.cc
	js/js_object_register.h
	js/js_object_register.cc
	js/js_state_wrapper.h
	js/js_state_wrapper.cc
	js/js_wrapper.h
	js/js_wrapper.cc
)

SET (CVarSources
	cvar/cvar.h
	cvar/cvar.cc
)

SET (ContentSources
	content/content.h
	content/content.cc
	content/content_manager.h
	content/content_manager.cc
)

SET (IOSources
	io/io_manager.h
	io/io_manager.cc
)

SOURCE_GROUP("win32" 			FILES ${Win32Sources})
SOURCE_GROUP("platform" 		FILES ${PlatformSources})
SOURCE_GROUP("application" 	FILES ${ApplicationSources})
SOURCE_GROUP("d3d11" 			FILES ${D3DSources})
SOURCE_GROUP("ogl" 				FILES ${OGLSources})
SOURCE_GROUP("memory" 		FILES ${MemorySources})
SOURCE_GROUP("input" 			FILES ${InputSources})
SOURCE_GROUP("js" 				FILES ${JavaScriptSources})
SOURCE_GROUP("cvar" 				FILES ${CVarSources})
SOURCE_GROUP("content" 		FILES ${ContentSources})
SOURCE_GROUP("io" 				FILES ${IOSources})

#Create the actual snuffbox project
SET (SNUFF_SOURCES
	${PlatformSources}
	${ApplicationSources}
	${MemorySources}
	${InputSources}
	${JavaScriptSources}
	${CVarSources}
	${ContentSources}
	${IOSources}
)

IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	SET (SNUFF_SOURCES ${SNUFF_SOURCES} ${OSXSources})
	ADD_DEFINITIONS (-DSNUFF_OSX)
ELSEIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	SET (SNUFF_SOURCES ${SNUFF_SOURCES} ${LinuxSources})
	ADD_DEFINITIONS (-DSNUFF_LINUX)
ELSEIF (WIN32)
	SET (SNUFF_SOURCES ${SNUFF_SOURCES} ${Win32Sources})
	ADD_DEFINITIONS (-DSNUFF_WIN32)
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

IF (WIN32 AND NOT SNUFF_BUILD_OPENGL)
	SET (SNUFF_SOURCES ${SNUFF_SOURCES} ${D3DSources})
ELSEIF (SNUFF_BUILD_OPENGL)
	SET (SNUFF_SOURCES ${SNUFF_SOURCES} ${OGLSources})
ELSE ()
	MESSAGE (FATAL_ERROR "Attempted to create a Mac OSX/Linux solution, but SNUFF_BUILD_OPENGL was set to OFF. Please turn on this flag")
ENDIF (WIN32 AND NOT SNUFF_BUILD_OPENGL)

SET (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
SET (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

IF (SNUFF_BUILD_CONSOLE)
	ADD_SUBDIRECTORY (console)
	SOURCE_GROUP("console" FILES ${ConsoleSources})
	
	SET (SNUFF_SOURCES ${SNUFF_SOURCES} ${ConsoleSources})
ENDIF (SNUFF_BUILD_CONSOLE)

ADD_EXECUTABLE(snuffbox ${SNUFF_SOURCES})

IF (WIN32)
	SET_TARGET_PROPERTIES (snuffbox PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:\"mainCRTStartup\"")
ENDIF (WIN32)

SET (SNUFF_BASE_LIBS "${V8_LIBRARIES};${FBX_LIBRARY};${FMOD_LIBRARY};${FREETYPE_LIBRARY}")
IF (SNUFF_BUILD_OPENGL)
	SET (SNUFF_LIBRARIES "${SNUFF_BASE_LIBS}")
ELSE()
	SET (SNUFF_LIBRARIES "${SNUFF_BASE_LIBS};${DirectX_D3D11_LIBRARIES}")
ENDIF (SNUFF_BUILD_OPENGL)

IF (SNUFF_BUILD_CONSOLE)
	SET (SNUFF_LIBRARIES "${SNUFF_LIBRARIES};Qt5::Widgets")
ENDIF (SNUFF_BUILD_CONSOLE)
TARGET_LINK_LIBRARIES (snuffbox ${SNUFF_LIBRARIES})