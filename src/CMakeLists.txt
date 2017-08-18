PROJECT (suite)

CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

# -----------------------------------------------------------------------------
# Release information (version number, code name, type).
# -----------------------------------------------------------------------------
SET (SOFTWARE_NAME "AERIE" CACHE STRING "Software package name" FORCE)
SET (SOFTWARE_MAJOR_VERSION 2 CACHE STRING "Major version number" FORCE)
SET (SOFTWARE_MINOR_VERSION 6 CACHE STRING "Minor version number" FORCE)
SET (SOFTWARE_PATCH_VERSION 0 CACHE STRING "Patch number" FORCE)
SET (SOFTWARE_CODENAME "Angry Giraffe" CACHE STRING "Release name" FORCE)

# Set the default build type to "Debug" if none is specified.
# Alternatives are: Release, MinSizeRel, and RelWithDebInfo
IF (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  MESSAGE (STATUS "Setting build type to 'Release' as none was specified.")
  SET (CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  SET_PROPERTY (CACHE CMAKE_BUILD_TYPE
    PROPERTY STRINGS "Debug" "Release" "FastRel" "MinSizeRel" "RelWithDebInfo")
ENDIF ()

# Remaining details for the build
INCLUDE (cmake/toplevel.cmake NO_POLICY_SCOPE)

