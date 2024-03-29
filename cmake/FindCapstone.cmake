# Locate the Capstone library
#
# This module defines:
#  Capstone_FOUND - system has CAPSTONE
#  CAPSTONE_INCLUDE_DIR - the CAPSTONE include directory
#  CAPSTONE_LIBRARY - link these to use CAPSTONE

find_package(PkgConfig QUIET)
pkg_check_modules(PC_CAPSTONE capstone)

find_path(CAPSTONE_INCLUDE_DIR
	capstone.h
	HINTS ${PC_CAPSTONE_INCLUDEDIR} ${PC_CAPSTONE_INCLUDE_DIRS}
	PATH_SUFFIXES capstone
)

find_library(CAPSTONE_LIBRARY
	NAMES capstone
	HINTS ${PC_CAPSTONE_LIBDIR} ${PC_CAPSTONE_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Capstone DEFAULT_MSG
                                  CAPSTONE_LIBRARY CAPSTONE_INCLUDE_DIR)

mark_as_advanced(CAPSTONE_INCLUDE_DIR CAPSTONE_LIBRARY)
