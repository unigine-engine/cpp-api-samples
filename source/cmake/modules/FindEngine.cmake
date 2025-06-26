#  Find `Engine` library.
#  Once done this will define:
#  UnigineExt::Engine - library target.
#  Engine_FOUND - library is found.
#  Engine_LIBRARY - library path.

set(name "Unigine")
set(suffix "_x64")

find_library(Engine_LIBRARY_RELEASE
    NAMES ${name}${suffix}
    PATHS ${UNIGINE_BIN_DIR} ${UNIGINE_LIB_DIR}
    NO_DEFAULT_PATH
    )
find_library(Engine_LIBRARY_DEBUG
    NAMES ${name}${suffix}d
    PATHS ${UNIGINE_BIN_DIR} ${UNIGINE_LIB_DIR}
    NO_DEFAULT_PATH
    )
find_library(Engine_LIBRARY_RELEASE_DOUBLE
    NAMES ${name}_double${suffix}
    PATHS ${UNIGINE_BIN_DIR} ${UNIGINE_LIB_DIR}
    NO_DEFAULT_PATH
    )
mark_as_advanced(Engine_LIBRARY_RELEASE_DOUBLE)
find_library(Engine_LIBRARY_DEBUG_DOUBLE
    NAMES ${name}_double${suffix}d
    PATHS ${UNIGINE_BIN_DIR} ${UNIGINE_LIB_DIR}
    NO_DEFAULT_PATH
    )
mark_as_advanced(Engine_LIBRARY_DEBUG_DOUBLE)

if (UNIGINE_DOUBLE)
    set(Engine_LIBRARY_RELEASE ${Engine_LIBRARY_RELEASE_DOUBLE})
    set(Engine_LIBRARY_DEBUG ${Engine_LIBRARY_DEBUG_DOUBLE})
endif()

include(SelectLibraryConfigurations)
select_library_configurations(Engine)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Engine
	FOUND_VAR Engine_FOUND
	REQUIRED_VARS
		Engine_LIBRARY
		UNIGINE_INCLUDE_DIR
	)

if(Engine_FOUND)
	add_library(Unigine::Engine UNKNOWN IMPORTED)
	set_target_properties(Unigine::Engine
		PROPERTIES
		INTERFACE_SYSTEM_INCLUDE_DIRECTORIES ${UNIGINE_INCLUDE_DIR}
		INTERFACE_INCLUDE_DIRECTORIES ${UNIGINE_INCLUDE_DIR}
		)

	set_property(TARGET Unigine::Engine
		PROPERTY
		INTERFACE_COMPILE_DEFINITIONS $<$<BOOL:${UNIGINE_DOUBLE}>:UNIGINE_DOUBLE>
		)

	set_target_properties(Unigine::Engine
		PROPERTIES
		IMPORTED_LOCATION ${Engine_LIBRARY_RELEASE}
		IMPORTED_LOCATION_DEBUG ${Engine_LIBRARY_DEBUG}
		IMPORTED_LOCATION_RELEASE ${Engine_LIBRARY_RELEASE}
		)
endif()
