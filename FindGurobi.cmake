# FindGurobi.cmake
# Locate GUROBI
# This module defines the following variables:
# GUROBI_FOUND, if false, do not try to link to GUROBI
# GUROBI_INCLUDE_DIRS, where to find the headers
# GUROBI_LIBRARY, the library to link against
# GUROBI_CXX_LIBRARY, the C++ library to link against
# GUROBI_CXX_DEBUG_LIBRARY, the debug C++ library to link against

find_path(GUROBI_INCLUDE_DIRS
        NAMES gurobi_c.h
        HINTS ${GUROBI_DIR} $ENV{GUROBI_HOME}
        PATH_SUFFIXES include)

find_library(GUROBI_LIBRARY
        NAMES gurobi gurobi100 gurobi110
        HINTS ${GUROBI_DIR} $ENV{GUROBI_HOME}
        PATH_SUFFIXES lib)

set(MSVC_YEAR "2017")

if(MT)
    set(M_FLAG "mt")
else()
    set(M_FLAG "md")
endif()

find_path(GUROBI_INCLUDE_DIRS
        NAMES gurobi_c.h
        HINTS ${GUROBI_DIR} $ENV{GUROBI_HOME}
        PATH_SUFFIXES include)

find_library(GUROBI_LIBRARY
        NAMES gurobi gurobi100 gurobi110
        HINTS ${GUROBI_DIR} $ENV{GUROBI_HOME}
        PATH_SUFFIXES lib)

message(STATUS "Found GUROBI home directory: $ENV{GUROBI_HOME}")
message(STATUS "Found GUROBI include directory: ${GUROBI_INCLUDE_DIRS}")
message(STATUS "Found GUROBI library: ${GUROBI_LIBRARY}")
message(STATUS "Found GUROBI C++ library: ${GUROBI_CXX_LIBRARY}")
message(STATUS "Found GUROBI C++ debug library: ${GUROBI_CXX_DEBUG_LIBRARY}")

set(MSVC_YEAR "2017")

if(MT)
    set(M_FLAG "mt")
else()
    set(M_FLAG "md")
endif()
find_library(GUROBI_CXX_LIBRARY
        NAMES gurobi_c++${M_FLAG}${MSVC_YEAR}
        HINTS ${GUROBI_DIR} $ENV{GUROBI_HOME}
        PATH_SUFFIXES lib)
find_library(GUROBI_CXX_DEBUG_LIBRARY
        NAMES gurobi_c++${M_FLAG}d${MSVC_YEAR}
        HINTS ${GUROBI_DIR} $ENV{GUROBI_HOME}
        PATH_SUFFIXES lib)



include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GUROBI DEFAULT_MSG GUROBI_LIBRARY)