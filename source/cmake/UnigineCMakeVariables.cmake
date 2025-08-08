# Copyright (C), UNIGINE. All rights reserved.

include_guard(GLOBAL)

# NOTE: Some of the following global variables must be set before the first
# `project` call, e.g. CMAKE_VS_WINDOWS_TARGET_<> variables are used to select
# Windows SDK during the `project` call.

if(NOT DEFINED UNIGINE_CMAKE_NOT_FIRST_RUN AND DEFINED CMAKE_PROJECT_NAME)
	message(FATAL_ERROR "This file must be included before the `project` call!")
endif()

set(UNIGINE_CMAKE_NOT_FIRST_RUN 1 CACHE INTERNAL "")
set(UNIGINE_VS_WINDOWS_TARGET_PLATFORM_MINIMUM_VERSION "10.0.19041.0")

##==============================================================================
## Setup CMake's global settings.
##==============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION_MAXIMUM FALSE)
set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION ${UNIGINE_VS_WINDOWS_TARGET_PLATFORM_MINIMUM_VERSION})

# Unigine doesn't have a distinction between BUILD/INSTALL binaries.
# That's why we need to compile builds with final RPATH right away.
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_SKIP_BUILD_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "$ORIGIN")
