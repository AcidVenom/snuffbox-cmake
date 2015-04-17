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
	platform/platform_render_device_base.h
	platform/platform_render_device.h
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
	d3d11/d3d11_render_target.h
	d3d11/d3d11_render_target.cc
	d3d11/d3d11_render_settings.h
	d3d11/d3d11_render_settings.cc
	d3d11/d3d11_viewport.h
	d3d11/d3d11_viewport.cc
	d3d11/d3d11_vertex_buffer.h
	d3d11/d3d11_vertex_buffer.cc
	d3d11/d3d11_shader.h
	d3d11/d3d11_shader.cc
	d3d11/d3d11_input_layout.h
	d3d11/d3d11_input_layout.cc
	d3d11/d3d11_render_queue.h
	d3d11/d3d11_render_queue.cc
	d3d11/d3d11_sampler_state.h
	d3d11/d3d11_sampler_state.cc
	d3d11/d3d11_camera.h
	d3d11/d3d11_camera.cc
	d3d11/d3d11_constant_buffer.h
	d3d11/d3d11_constant_buffer.cc
	d3d11/d3d11_blend_state.h
	d3d11/d3d11_blend_state.cc
	d3d11/d3d11_depth_state.h
	d3d11/d3d11_depth_state.cc
	d3d11/d3d11_effect.h
	d3d11/d3d11_effect.cc
	d3d11/d3d11_material.h
	d3d11/d3d11_material.cc
	d3d11/d3d11_texture.h
	d3d11/d3d11_texture.cc
	d3d11/d3d11_light.h
	d3d11/d3d11_light.cc
	d3d11/d3d11_lighting.h
	d3d11/d3d11_lighting.cc
	d3d11/d3d11_line.h
	d3d11/d3d11_line.cc
	d3d11/d3d11_uniforms.h
	d3d11/d3d11_uniforms.cc
	d3d11/d3d11_rasterizer_state.h
	d3d11/d3d11_rasterizer_state.cc
)

SET (D3DElements
	d3d11/elements/d3d11_render_element.h
	d3d11/elements/d3d11_render_element.cc
	d3d11/elements/d3d11_quad_element.h
	d3d11/elements/d3d11_quad_element.cc
	d3d11/elements/d3d11_terrain_element.h
	d3d11/elements/d3d11_terrain_element.cc
	d3d11/elements/d3d11_model_element.h
	d3d11/elements/d3d11_model_element.cc
	d3d11/elements/d3d11_widget_element.h
	d3d11/elements/d3d11_widget_element.cc
	d3d11/elements/d3d11_billboard_element.h
	d3d11/elements/d3d11_billboard_element.cc
	d3d11/elements/d3d11_text_element.h
	d3d11/elements/d3d11_text_element.cc
	d3d11/elements/d3d11_polygon_element.h
	d3d11/elements/d3d11_polygon_element.cc
)

SET (D3DShaders
	d3d11/shaders/d3d11_base_shader.h
	d3d11/shaders/d3d11_ui_shader.h
	d3d11/shaders/d3d11_text_shader.h
	d3d11/shaders/d3d11_post_processing_shader.h
	d3d11/shaders/d3d11_cube_map_shader.h
	d3d11/shaders/d3d11_brush_shader.h
	d3d11/shaders/d3d11_base_diffuse_shader.h
	d3d11/shaders/d3d11_post_processing_diffuse_shader.h
	d3d11/shaders/base.txt
	d3d11/shaders/post_processing.txt
	d3d11/shaders/ui.txt
	d3d11/shaders/text.txt
	d3d11/shaders/cube_map.txt
	d3d11/shaders/brush.txt
	d3d11/shaders/base_diffuse.txt
	d3d11/shaders/post_processing_diffuse.txt
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
	input/mouse_area.h
	input/mouse_area.cc
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
	content/content_box.h
	content/content_box.cc
)

SET (IOSources
	io/io_manager.h
	io/io_manager.cc
)

SET (FBXSources
	fbx/fbx_loader.h
	fbx/fbx_loader.cc
	fbx/fbx_model.h
	fbx/fbx_model.cc
)

SET (FTSources
	freetype/freetype_font.h
	freetype/freetype_font.cc
	freetype/freetype_font_atlas.h
	freetype/freetype_font_atlas.cc
	freetype/freetype_font_manager.h
	freetype/freetype_font_manager.cc
	freetype/freetype_rich_text.h
	freetype/freetype_rich_text.cc
)

SET (FMODSources
	fmod/fmod_sound.h
	fmod/fmod_sound.cc
	fmod/fmod_sound_system.h
	fmod/fmod_sound_system.cc
)

SET (AnimationSources
	animation/animation_base.h
	animation/sprite_animation.h
	animation/sprite_animation.cc
	animation/anim.h
	animation/anim.cc
)

SOURCE_GROUP("win32" 			FILES ${Win32Sources})
SOURCE_GROUP("platform" 		FILES ${PlatformSources})
SOURCE_GROUP("application" 	FILES ${ApplicationSources})
SOURCE_GROUP("d3d11" 			FILES ${D3DSources})
SOURCE_GROUP("d3d11\\elements" 	FILES ${D3DElements})
SOURCE_GROUP("d3d11\\shaders" 		FILES ${D3DShaders})
SOURCE_GROUP("ogl" 				FILES ${OGLSources})
SOURCE_GROUP("memory" 		FILES ${MemorySources})
SOURCE_GROUP("input" 			FILES ${InputSources})
SOURCE_GROUP("js" 				FILES ${JavaScriptSources})
SOURCE_GROUP("cvar" 				FILES ${CVarSources})
SOURCE_GROUP("content" 		FILES ${ContentSources})
SOURCE_GROUP("io" 				FILES ${IOSources})
SOURCE_GROUP("fbx" 				FILES ${FBXSources})
SOURCE_GROUP("freetype" 		FILES ${FTSources})
SOURCE_GROUP("animation"		FILES ${AnimationSources})
SOURCE_GROUP("fmod"		FILES ${FMODSources})

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
	${FBXSources}
	${FTSources}
	${FMODSources}
	${AnimationSources}
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
	SET (SNUFF_SOURCES ${SNUFF_SOURCES} ${D3DElements})
	SET (SNUFF_SOURCES ${SNUFF_SOURCES} ${D3DShaders})
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