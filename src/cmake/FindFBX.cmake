GET_FILENAME_COMPONENT(module_file_path ${CMAKE_CURRENT_LIST_FILE} PATH )

# Look for the header file
FIND_PATH( FBX_INCLUDE NAMES fbxsdk.h
           PATHS "${FBX_INCLUDE_DIR}"
           DOC "Path in which the file fbx.h is located" )

# Look for the library
SET (FBX_LIBD "${FBX_LIBRARY_DIR}/debug")
SET (FBX_LIB "${FBX_LIBRARY_DIR}/release")

FIND_LIBRARY( FBX_LIB_DEBUG NAMES libfbxsdk-mt.lib
	PATHS "${FBX_LIBD}"
	DOC "Path to where fbx debug library is located" )
	
FIND_LIBRARY( FBX_LIB_RELEASE NAMES libfbxsdk-mt.lib
	PATHS "${FBX_LIB}"
	DOC "Path to where fbx library is located" )

SET ( FBX_LIBRARY
	debug "${FBX_LIB_DEBUG}"
	optimized "${FBX_LIB_RELEASE}"
)

IF (FBX_LIBRARY)
	SET (FBX_FOUND 1)
ELSE()
	SET (FBX_FOUND 0)
ENDIF (FBX_LIBRARY)

IF(NOT FBX_FOUND)
  SET(FBX_MESSAGE
    "The Autodesk FBX SDK was not found. Make sure FBX_LIBRARY_DIR and FBX_INCLUDE_DIR are set")
  IF(FBX_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "${FBX_MESSAGE}")
  ELSEIF(NOT FBX_FIND_QUIETLY)
    MESSAGE(STATUS "${FBX_MESSAGE}")
  ENDIF(FBX_FIND_REQUIRED)
ENDIF(NOT FBX_FOUND)