
# -----------------------------------------------------------------------------
# Prevent in-place builds before starting anything
# -----------------------------------------------------------------------------
IF ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  MESSAGE (FATAL_ERROR "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                             HAWCNest Build Warning:
     Please do not try building inside the source tree; it creates a mess!

 Instructions:
   1) rm -rf CMakeFiles CMakeCache.txt
   2) cd ../build
   3) cmake -DCMAKE_INSTALL_PREFIX=/path/to/install ../src
   4) make && make install
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
ENDIF ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")

# -----------------------------------------------------------------------------
# Set up CMake policies (if any)
# -----------------------------------------------------------------------------
IF (COMMAND cmake_policy)
  CMAKE_POLICY(SET CMP0009 NEW)  # Do not follow symlinks w/ FILE_GLOB_RECURSE

  IF ("${CMAKE_VERSION}" VERSION_GREATER 2.8.12)
    CMAKE_POLICY(SET CMP0026 OLD)  # Read LOCATION properties from build targets
    CMAKE_POLICY(SET CMP0042 OLD)  # Disable @rpath in target install name
  ENDIF()

ENDIF (COMMAND cmake_policy)

# -----------------------------------------------------------------------------
# Set up build paths and external packages
# -----------------------------------------------------------------------------
SET (CMAKE_MODULE_PATH
  ${CMAKE_SOURCE_DIR}/cmake
  ${CMAKE_SOURCE_DIR}/cmake/Utility
  ${CMAKE_SOURCE_DIR}/cmake/Packages)

SET (CMAKE_SKIP_BUILD_RPATH FALSE)

INCLUDE (Utility)
INCLUDE (ProjectMacros)

INCLUDE (Root)
INCLUDE (Python)
INCLUDE (Boost)
INCLUDE (FFTW3)
INCLUDE (CFITSIO)
INCLUDE (HEALPix)
INCLUDE (GSL)
INCLUDE (PhotoSpline)
INCLUDE (XercesC)
INCLUDE (Sphinx)
INCLUDE (3ML)

INCLUDE(CheckCXXCompilerFlag)

# Configuration switch to enable C++11 support (off by default)
OPTION (ENABLE_CXX11 "Build in support for C++11" OFF)

IF (ENABLE_CXX11 OR (${ROOT_VERSION} VERSION_GREATER 5.99.99) )
  IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR CMAKE_COMPILER_IS_GNUCXX)
    CHECK_CXX_COMPILER_FLAG(--std=c++11 SUPPORTS_STD_CXX11)
    IF (SUPPORTS_STD_CXX11)
      SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")
      COLORMSG (GREEN "Enabling C++11 language features.")
    ELSE ()
      IF (${ROOT_VERSION} VERSION_GREATER 5.99.99)
        COLORMSG (RED "Root 6 and later require C++ 11.")
        MESSAGE (FATAL_ERROR "Aborting...")
      ENDIF ()
      CHECK_CXX_COMPILER_FLAG(--std=c++0x SUPPORTS_STD_CXX01)
      IF (SUPPORTS_STD_CXX01)
        SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++0x")
        COLORMSG (GREEN "Enabling some C++11 language features.")
      ELSE ()
        COLORMSG (CYAN "Compiler does not support --std=c++11 or --std=c++0x.")
      ENDIF ()
    ENDIF ()
  ENDIF ()
ENDIF ()

# Look for doxygen to make documentation
FIND_PACKAGE (Doxygen)

# Try to get a revision number for the repository
REPOSITORY_REVISION (${CMAKE_SOURCE_DIR})

# -----------------------------------------------------------------------------
# Set variables useful for building against the aerie distribution:
#   + PLATFORM_LD_LIBRARY_PATH
#   + HAWC_CPPFLAGS
#   + HAWC_LDFLAGS
##   + HAWC_BUILD
#   + HAWC_SRC
#   + HAWC_INSTALL, HAWC_BINDIR, HAWC_LIBDIR, HAWC_INCDIR
# -----------------------------------------------------------------------------
IF (CMAKE_HOST_APPLE)
  SET (PLATFORM_LD_LIBRARY_PATH "DYLD_LIBRARY_PATH")
ELSE (CMAKE_HOST_APPLE)
  SET (PLATFORM_LD_LIBRARY_PATH "LD_LIBRARY_PATH")
ENDIF (CMAKE_HOST_APPLE)

# Set up CMake build flags
SET (gen_cxxflags "-fno-strict-aliasing -DNOMARS -DCPLUSPLUS_OK -DHAWCNEST ${PLATFORM_FLAGS}")
SET (warn_cxxflags "-Wall -Wno-unused-function -Wno-unknown-pragmas")

# Turn off verbose warnings about unused local typedef warnings
IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # require at least gcc 4.8
  IF (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.7)
    CHECK_CXX_COMPILER_FLAG(-Wno-unused-local-typedef HAS_NO_UNUSED_LOCAL_TYPEDEF)
    IF (HAS_NO_UNUSED_LOCAL_TYPEDEF)
      SET (warn_cxxflags "${warn_cxxflags} -Wno-unused-local-typedef")
    ENDIF()
  ENDIF()
ELSEIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  # require at least clang 3.6
  IF (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 3.5)
    CHECK_CXX_COMPILER_FLAG(-Wno-unused-local-typedef HAS_NO_UNUSED_LOCAL_TYPEDEF)
    IF (HAS_NO_UNUSED_LOCAL_TYPEDEF)
      SET (warn_cxxflags "${warn_cxxflags} -Wno-unused-local-typedef")
    ENDIF ()
  ENDIF()
ELSE()
ENDIF()

# Debug mode flags
SET (CMAKE_CXX_FLAGS_DEBUG
  "-O0 -g ${warn_cxxflags} ${gen_cxxflags}"
  CACHE STRING "Debug-mode flags" FORCE)

# Release mode flags
SET (CMAKE_CXX_FLAGS_RELEASE
  "-O3 -DNDEBUG ${warn_cxxflags} ${gen_cxxflags}"
  CACHE STRING "Release-mode flags" FORCE)

# Release mode flags with debugging info (suboptimal for debugging)
SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO
  "-O2 -DNDEBUG -g ${warn_cxxflags} ${gen_cxxflags}"
  CACHE STRING "Release-mode flags with debugging enabled" FORCE)

# Release mode flags (optimized for size)
SET (CMAKE_CXX_FLAGS_MINSIZEREL
  "-Os -DNDEBUG ${warn_cxxflags} ${gen_cxxflags}"
  CACHE STRING "Minimum size release-mode flags" FORCE)

# Release mode flags, optimized for speed (may violate language standards)
# Note that -Ofast is supported by clang but not older versions of gcc
check_cxx_compiler_flag(-Ofast HAS_OFAST)
IF (HAS_OFAST)
  SET (CMAKE_CXX_FLAGS_FASTREL
    "-Ofast -DNDEBUG ${warn_cxxflags} ${gen_cxxflags}"
    CACHE STRING "Release-mode flags" FORCE)
ELSE ()
  SET (CMAKE_CXX_FLAGS_FASTREL
    "-O3 -ffast-math -march=native -DNDEBUG ${warn_cxxflags} ${gen_cxxflags}"
    CACHE STRING "Release-mode flags" FORCE)
ENDIF ()

SET (HAWC_BUILD "${CMAKE_BINARY_DIR}")
SET (HAWC_SRC "${CMAKE_SOURCE_DIR}")
SET (HAWC_INSTALL "${CMAKE_INSTALL_PREFIX}")
SET (HAWC_BINDIR "${CMAKE_INSTALL_PREFIX}/bin")
SET (HAWC_LIBDIR "${CMAKE_INSTALL_PREFIX}/lib")
SET (HAWC_INCDIR "${CMAKE_INSTALL_PREFIX}/include")
SET (HAWC_SHARE "${CMAKE_INSTALL_PREFIX}/share/hawc")
SET (HAWC_CONFIG "${CMAKE_INSTALL_PREFIX}/share/hawc/config")
SET (HAWC_CPPFLAGS "${CMAKE_CXX_FLAGS_RELEASE} -I${HAWC_INCDIR}")
IF(QT_FOUND)
  SET (HAWC_CPPFLAGS "${HAWC_CPPFLAGS} -DBUILD_WITH_QT")
  ADD_DEFINITIONS(-DBUILD_WITH_QT)
ENDIF(QT_FOUND)
SET (HAWC_LDFLAGS "-L${HAWC_LIBDIR}")

# Support OpenMP parallelization if -DUSE_OPENMP=ON is specified
FIND_PACKAGE(OpenMP)
IF (OPENMP_FOUND)
  ADD_DEFINITIONS(${OpenMP_CXX_FLAGS})
  SET (CMAKE_EXE_LINKER_FLAGS
       "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_CXX_FLAGS}"
       CACHE STRING "EXE linker flags." FORCE)
  SET (CMAKE_SHARED_LINKER_FLAGS
       "${CMAKE_SHARED_LINKER_FLAGS} ${OpenMP_CXX_FLAGS}"
       CACHE STRING "SHARED link flags." FORCE)
  SET (CMAKE_MODULE_LINKER_FLAGS
       "${CMAKE_MODULE_LINKER_FLAGS} ${OpenMP_CXX_FLAGS}"
       CACHE STRING "MODULE link flags." FORCE)
  SET (HPX_CPPFLAGS "${HPX_CPPFLAGS} ${OpenMP_CXX_FLAGS}")
  SET (HPX_LDFLAGS "${HPX_LDFLAGS} ${OpenMP_CXX_FLAGS}")
ELSE ()
  MESSAGE(STATUS "OpenMP not found ..")
ENDIF ()

INCLUDE (config)

GET_PROPERTY (_defs DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY COMPILE_DEFINITIONS)

# Specify any building options that can be turned on or off
OPTION (ENABLE_PYTHON_BINDINGS "Build python bindings" ON)

# Enable CTest
ENABLE_TESTING()

#  Glob together a list of subdirectories containing a CMakeLists.txt
FILE (GLOB cmake_projects
  RELATIVE ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/*/CMakeLists.txt)
FILE (GLOB hidden_projects
  RELATIVE ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/.*/CMakeLists.txt)
FILE (GLOB runlast_projects
  RELATIVE ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/*/RUNLAST)

IF (hidden_projects)
  LIST (REMOVE_ITEM cmake_projects ${hidden_projects})
ENDIF (hidden_projects)

IF (runlast_projects)
  FOREACH (rm_proj ${runlast_projects})
    string(REGEX REPLACE "RUNLAST" "CMakeLists.txt" rm_proj ${rm_proj})
    LIST (REMOVE_ITEM cmake_projects ${rm_proj} )
    MESSAGE (STATUS "Moving ${rm_proj} to end of projects list.")
    SET (cmake_projects ${cmake_projects} ${rm_proj} )
  ENDFOREACH ()
ENDIF (runlast_projects)

FOREACH (d ${cmake_projects})
  GET_FILENAME_COMPONENT (_proj ${d} PATH)
  SET (SUBDIRS ${SUBDIRS} ${_proj})
  GET_FILENAME_COMPONENT (_pname ${_proj} NAME_WE)
  SET (PROJECT_NAMES ${PROJECT_NAMES} ${_pname})
  SET (HAVE_${_pname} TRUE)
ENDFOREACH (d ${cmake_projects})

MESSAGE (STATUS "Configuring projects:")
HAWC_PROJECT (cmake DOCS_DIR cmake/resources)

FOREACH (subdir ${SUBDIRS})
  GET_FILENAME_COMPONENT (pname ${subdir} NAME_WE)
  COLORMSG (GREEN "  ${pname}")
  ADD_SUBDIRECTORY (${CMAKE_SOURCE_DIR}/${pname})

  # Set up the SUBDIRSINC list for doxygen
  FILE (GLOB project_sub_dirs ${CMAKE_SOURCE_DIR}/${pname}/*)
  FOREACH (sd ${project_sub_dirs})
    IF (IS_DIRECTORY ${sd})
      STRING (REGEX MATCH ".*\\.svn" _is_svn ${sd})
      IF (NOT _is_svn)
        SET (SUBDIRSINC "${SUBDIRSINC} ${sd}")
      ENDIF ()
    ENDIF ()
  ENDFOREACH ()

ENDFOREACH (subdir ${SUBDIRS})

# Doxyfile
IF (DOXYGEN_FOUND AND SPHINX_BUILD)
  # Create doxygen target
  MESSAGE (STATUS "Doxygen found.  Building docs target")
#  set(TAGFILES
#"${CMAKE_SOURCE_DIR}/cmake/libstdc++.tag=http://gcc.gnu.org/onlinedocs/libstdc++/latest-doxygen")
  SET (SUBDIRSINC "${SUBDIRSINC} ${CMAKE_SOURCE_DIR}/docs/doxygen")
  SET (SUBDIRSINC "${SUBDIRSINC} ${CMAKE_BINARY_DIR}/docs/doxygen")
  MESSAGE (STATUS "doxygen directory: ${CMAKE_SOURCE_DIR}/docs/doxygen")
  CONFIGURE_FILE (
    ${CMAKE_SOURCE_DIR}/docs/doxygen/doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/doxyfile
    @ONLY)
  CONFIGURE_FILE (
    ${CMAKE_SOURCE_DIR}/docs/doxygen/index.dox.in
    ${CMAKE_BINARY_DIR}/docs/doxygen/index.dox
    @ONLY)
  ADD_CUSTOM_TARGET(doxygen ${DOXYGEN_EXECUTABLE})

  # Create sphinx target(s)
  CONFIGURE_FILE (
    ${CMAKE_SOURCE_DIR}/docs/source/conf.py.in
    ${CMAKE_SOURCE_DIR}/docs/source/conf.py
    @ONLY)

  ADD_CUSTOM_TARGET(html
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/docs ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/doctrees
    COMMAND ln -fsn
    ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/project-doc-links ${CMAKE_SOURCE_DIR}/docs/source/projects
    COMMAND ${SPHINX_BUILD} ${SPHINX_EXTRA_OPTIONS}
    -b html
    -d ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/doctrees
    -D latex_paper_size=${SPHINX_PAPER_SIZE}
    source
    ${CMAKE_BINARY_DIR}/docs
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs
    COMMENT "Generating docs in ${CMAKE_BINARY_DIR}/docs")

  ADD_CUSTOM_TARGET(docs)
  ADD_DEPENDENCIES(docs doxygen html)
ELSE ()
  MESSAGE (STATUS "Doxygen not found.  Not building docs target")
ENDIF ()

# ------------------------------------------------------------------------------
# Create a script for controlling the shell environment
# ------------------------------------------------------------------------------
MESSAGE("-- Creating hawc-config script")
CONFIGURE_FILE (${CMAKE_SOURCE_DIR}/cmake/hawc-config.in
  ${CMAKE_BINARY_DIR}/hawc-config
  @ONLY)
EXECUTE_PROCESS (COMMAND chmod +x ${CMAKE_BINARY_DIR}/hawc-config)
INSTALL (PROGRAMS ${CMAKE_BINARY_DIR}/hawc-config DESTINATION bin
         PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                     GROUP_READ GROUP_WRITE GROUP_EXECUTE
                     WORLD_READ             WORLD_EXECUTE)

# ------------------------------------------------------------------------------
# Add uninstall target, for running "make uninstall" in the build directory
# ------------------------------------------------------------------------------
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")

# ------------------------------------------------------------------------------
# Packaging control - version and name at beginning of package tarball
# ------------------------------------------------------------------------------
STRING (TOLOWER "${SOFTWARE_NAME}" CPACK_PACKAGE_NAME)
SET (CPACK_PACKAGE_VENDOR "HAWC Observatory Software Task")
SET (CPACK_PACKAGE_VERSION_MAJOR "${SOFTWARE_MAJOR_VERSION}")
PAD_STRING (CPACK_PACKAGE_VERSION_MINOR 2 0 ${SOFTWARE_MINOR_VERSION})
PAD_STRING (CPACK_PACKAGE_VERSION_PATCH 2 0 ${SOFTWARE_PATCH_VERSION})
SET (CPACK_SOURCE_GENERATOR "TBZ2")
# Beware: The list of ignored files is expanded twice. Take this into account
# when escaping special characters
SET (CPACK_SOURCE_IGNORE_FILES "[.]svn;[.]setting;[.].*project;[.]git.*")
#
# Agreed-upon package names:
#  - aerie-trunk-rev12345.tar.bz2 .............. builds from trunk
#  - aerie-0.XX-br.tar.bz2 ..................... builds from branch
#  - aerie-0.XX.YY-(beta|rc|release).tar.bz2 ... beta, candidate, or release
#  - aerie-0.XX.YY.tar.bz2 ..................... full release
#
IF ("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
  IF ("${SOFTWARE_REPOSITORY_REVISION}" MATCHES "unknown")
    SET (CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-trunk")
  ELSE ()
    SET (CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-trunk-rev${SOFTWARE_REPOSITORY_REVISION}")
  ENDIF ()
ELSE ()
  SET (CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${CMAKE_BUILD_TYPE}")
ENDIF ()
INCLUDE (CPack)

# ------------------------------------------------------------------------------
# Post-installation: run scripts in this folder after installation is complete
# ------------------------------------------------------------------------------
ADD_SUBDIRECTORY (${CMAKE_SOURCE_DIR}/cmake/PostInstall)

