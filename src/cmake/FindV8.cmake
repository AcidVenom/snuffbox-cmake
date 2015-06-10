GET_FILENAME_COMPONENT(module_file_path ${CMAKE_CURRENT_LIST_FILE} PATH )

# Look for the header file
FIND_PATH( V8_INCLUDE NAMES v8.h
           PATHS "${V8_INCLUDE_DIR}"
           DOC "Path in which the file v8.h is located" )

# Look for the library
SET (V8_LIBD "${V8_LIBRARY_DIR}/Debug/lib")
SET (V8_LIB "${V8_LIBRARY_DIR}/Release/lib")

FIND_LIBRARY( V8_BASE_DEBUG_LIB NAMES v8_base.x64.lib
	PATHS "${V8_LIBD}"
	DOC "Path to where v8 base debug is located" )
	
FIND_LIBRARY( V8_BASE_LIB NAMES v8_base.x64.lib
	PATHS "${V8_LIB}"
	DOC "Path to where v8 base is located" )

SET ( V8_LIBRARY_BASE 
	debug "${V8_BASE_DEBUG_LIB}"
	optimized "${V8_BASE_LIB}"
)
	
SET( V8_LIBRARY_NOSNAPSHOT 
	debug "${V8_LIBD}/v8_nosnapshot.x64.lib" 
	optimized "${V8_LIB}/v8_nosnapshot.x64.lib" )

SET( V8_LIBRARY_SNAPSHOT 
	debug "${V8_LIBD}/v8_snapshot.lib" 
	optimized "${V8_LIB}/v8_snapshot.lib" )

SET( V8_LIBRARY_ICUI18N
	debug "${V8_LIBD}/icui18n.lib" 
	optimized "${V8_LIB}/icui18n.lib" )

SET( V8_LIBRARY_ICUUC
	debug "${V8_LIBD}/icuuc.lib" 
	optimized "${V8_LIB}/icuuc.lib" )

SET( V8_LIBRARY_LIBBASE
	debug "${V8_LIBD}/v8_libbase.lib" 
	optimized "${V8_LIB}/v8_libbase.lib" )

SET( V8_LIBRARY_LIBPLATFORM
	debug "${V8_LIBD}/v8_libplatform.lib" 
	optimized "${V8_LIB}/v8_libplatform.lib" )
	
SET (V8_LIBRARIES
	${V8_LIBRARY_ICUUC}
	${V8_LIBRARY_ICUI18N}
	${V8_LIBRARY_BASE}
	${V8_LIBRARY_SNAPSHOT}
	${V8_LIBRARY_NOSNAPSHOT}
	${V8_LIBRARY_LIBBASE}
	${V8_LIBRARY_LIBPLATFORM}
	WINMM.lib;
)

IF (V8_INCLUDE AND V8_BASE_LIB AND V8_BASE_DEBUG_LIB)
	SET (V8_FOUND 1)
ELSE()
	SET (V8_FOUND 0)
ENDIF (V8_INCLUDE AND V8_BASE_LIB AND V8_BASE_DEBUG_LIB)

IF(NOT V8_FOUND)
  SET(V8_DIR_MESSAGE
    "V8 was not found. Make sure V8_LIBRARY_DIR and V8_INCLUDE_DIR are set")
  IF(V8_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "${V8_DIR_MESSAGE}")
  ELSEIF(NOT V8_FIND_QUIETLY)
    MESSAGE(STATUS "${V8_DIR_MESSAGE}")
  ENDIF(V8_FIND_REQUIRED)
ENDIF(NOT V8_FOUND)