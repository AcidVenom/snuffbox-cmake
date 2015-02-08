SET (Win32Sources 
	win32/win32_window.cc
	win32/win32_window.h
	win32/win32_file_watch.cc
	win32/win32_file_watch.h
)

SET (PlatformSources
	platform/platform_window.h
	platform/platform_file_watch.h
)

SOURCE_GROUP("win32" 		FILES ${Win32Sources})
SOURCE_GROUP("platform" 	FILES ${PlatformSources})

#Create the actual snuffbox project
SET (SNUFF_SOURCES
	"main.cc"
	${Win32Sources}
	${PlatformSources}
)

SET (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
SET (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
ADD_EXECUTABLE(snuffbox ${SNUFF_SOURCES})

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