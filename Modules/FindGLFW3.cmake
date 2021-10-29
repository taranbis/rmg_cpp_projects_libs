# Locate the glfw3 library
#
# This module defines the following variables:
#
# GLFW3_LIBRARY the name of the library;
# GLFW3_INCLUDE_DIR where to find glfw include files.
# GLFW3_FOUND true if both the GLFW3_LIBRARY and GLFW3_INCLUDE_DIR have been found.
#
# To help locate the library and include file, you can define a
# variable called GLFW3_ROOT which points to the root of the glfw library
# installation.
#
# default search dirs
# 
# Cmake file from: https://github.com/daw42/glslcookbook

set( _glfw3_HEADER_SEARCH_DIRS
"/usr/include"
"/usr/local/include"
"${CMAKE_SOURCE_DIR}/includes"
"C:/Program Files (x86)/glfw/include" )
set( _glfw3_LIB_SEARCH_DIRS
"/usr/lib"
"/usr/local/lib"
"${CMAKE_SOURCE_DIR}/lib"
"C:/Program Files (x86)/glfw/lib-msvc110" )

# Check environment for root search directory
set( _glfw3_ENV_ROOT $ENV{GLFW3_ROOT} )
if( NOT GLFW3_ROOT AND _glfw3_ENV_ROOT )
	set(GLFW3_ROOT ${_glfw3_ENV_ROOT} )
endif()

# Put user specified location at beginning of search
if( GLFW3_ROOT )
	list( INSERT _glfw3_HEADER_SEARCH_DIRS 0 "${GLFW3_ROOT}/include" )
	list( INSERT _glfw3_LIB_SEARCH_DIRS 0 "${GLFW3_ROOT}/lib" )
endif()

# Search for the header
FIND_PATH(GLFW3_INCLUDE_DIR "GLFW/glfw3.h"
PATHS ${_glfw3_HEADER_SEARCH_DIRS} )

# Search for the library
FIND_LIBRARY(GLFW3_LIBRARY NAMES glfw3 glfw
PATHS ${_glfw3_LIB_SEARCH_DIRS} )

if (WIN32 AND GLFW3_LIBRARY)
	find_file(GLFW3_RUNTIME_LIBRARY
		NAMES
		glfw3.dll
		PATHS ${_glfw3_LIB_SEARCH_DIRS}
	)
endif (WIN32 AND GLFW3_LIBRARY)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLFW3 DEFAULT_MSG
GLFW3_LIBRARY GLFW3_INCLUDE_DIR)

if (glfw3_FOUND)
	if (NOT TARGET glfw3)
		add_library(glfw3 SHARED IMPORTED)
		if (WIN32)
			set_target_properties(glfw3
				PROPERTIES
				IMPORTED_IMPLIB "${GLFW3_LIBRARY}"
				IMPORTED_LOCATION "${GLFW3_RUNTIME_LIBRARY}"
			)
		else (WIN32)
			set_target_properties(glfw3
				PROPERTIES
				IMPORTED_LOCATION "${GLFW3_LIBRARY}"
			)
		endif (WIN32)
		set_target_properties(glfw3
			PROPERTIES
			INTERFACE_INCLUDE_DIRECTORIES "${GLFW3_INCLUDE_DIR}"
		)
	endif (NOT TARGET glfw3)
endif (glfw3_FOUND)

MARK_AS_ADVANCED(GLFW3_INCLUDE_DIR GLFW3_LIBRARY)
