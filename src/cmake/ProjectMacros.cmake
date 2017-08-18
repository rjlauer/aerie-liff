#
# (c) 2010, 2011   Segev BenZvi <sybenzvi@icecube.wisc.edu>
#
# The following are modified macros from the IceTray build system, with a lot
# of cleanup.  The IceTray license is produced below.
#  
#  Copyright (C) 2007   Troy D. Straszheim  <troy@icecube.umd.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#  
#  This file is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>
# 

# ------------------------------------------------------------------------------
# Macro PARSE_ARGUMENTS
# Take the arguments of another macro and define variable names such as
# prefix_OPTION1, prefix_OPTION2, etc.
#
# Copied from http://www.cmake.org/Wiki/CMakeMacroParseArguments
# ------------------------------------------------------------------------------
MACRO(PARSE_ARGUMENTS prefix arg_names option_names)
  SET(DEFAULT_ARGS)
  FOREACH(arg_name ${arg_names})    
    SET(${prefix}_${arg_name})
  ENDFOREACH(arg_name)
  FOREACH(option ${option_names})
    SET(${prefix}_${option} FALSE)
  ENDFOREACH(option)

  SET(current_arg_name DEFAULT_ARGS)
  SET(current_arg_list)
  FOREACH(arg ${ARGN})            
    SET(larg_names ${arg_names})    
    LIST(FIND larg_names "${arg}" is_arg_name)                   
    IF (is_arg_name GREATER -1)
      SET(${prefix}_${current_arg_name} ${current_arg_list})
      SET(current_arg_name ${arg})
      SET(current_arg_list)
    ELSE (is_arg_name GREATER -1)
      SET(loption_names ${option_names})    
      LIST(FIND loption_names "${arg}" is_option)            
      IF (is_option GREATER -1)
       SET(${prefix}_${arg} TRUE)
      ELSE (is_option GREATER -1)
       SET(current_arg_list ${current_arg_list} ${arg})
      ENDIF (is_option GREATER -1)
    ENDIF (is_arg_name GREATER -1)
  ENDFOREACH(arg)
  SET(${prefix}_${current_arg_name} ${current_arg_list})
ENDMACRO(PARSE_ARGUMENTS)

# ------------------------------------------------------------------------------
# Macro: USE_PROJECTS
# Parse a list of project name arguments and set up include and link
# directives needed by a target project
# ------------------------------------------------------------------------------
MACRO (USE_PROJECTS MY_PROJECT)
  PARSE_ARGUMENTS (${MY_PROJECT}_USE_PROJECTS
    "PROJECTS"
    ""
    ${ARGN})

  FOREACH (USED_PROJECT ${${MY_PROJECT}_USE_PROJECTS_PROJECTS})
    # Check project directory
    IF (NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${USED_PROJECT})
      MESSAGE (FATAL_ERROR "Project '${USED_PROJECT}' does not exist")
    ENDIF (NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${USED_PROJECT})

    # Check project includes
    IF (NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${USED_PROJECT}/include/${USED_PROJECT})
      MESSAGE (FATAL_ERROR "Project '${USED_PROJECT}' does not contain a proper"
               " public directory ${USED_PROJECT}/include/${USED_PROJECT}")
    ENDIF (NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${USED_PROJECT}/include/${USED_PROJECT})

    # Check project CMakeLists.txt
    IF (NOT EXISTS ${CMAKE_SOURCE_DIR}/${USED_PROJECT}/CMakeLists.txt)
      MESSAGE (FATAL_ERROR "Project '${USED_PROJECT}' has no CMakeLists.txt")
    ENDIF (NOT EXISTS ${CMAKE_SOURCE_DIR}/${USED_PROJECT}/CMakeLists.txt)

    # Set the include and link directives
    INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR}/${USED_PROJECT}/include)
    INCLUDE_DIRECTORIES (${CMAKE_BINARY_DIR}/${USED_PROJECT}/include)
    TARGET_LINK_LIBRARIES (${MY_PROJECT} ${USED_PROJECT})

  ENDFOREACH (USED_PROJECT ${${MY_PROJECT}_USE_PROJECTS_PROJECTS})
  TARGET_LINK_LIBRARIES (${MY_PROJECT} dl util pthread)

ENDMACRO (USE_PROJECTS MY_PROJECT)

# ------------------------------------------------------------------------------
# Macro: USE_PACKAGES
# Parse a list of external package names and set up include and link directives
# needed by a target project
# ------------------------------------------------------------------------------
MACRO (USE_PACKAGES MY_PROJECT)
  PARSE_ARGUMENTS (${MY_PROJECT}_USE_PACKAGES
    "PACKAGES"
    ""
    ${ARGN})

  FOREACH (_package ${${MY_PROJECT}_USE_PACKAGES_PACKAGES})
    STRING (TOUPPER ${_package} USED_PACKAGE)
    # Check for the existence of the package
    IF (NOT ${USED_PACKAGE}_FOUND)
      MESSAGE (FATAL_ERROR "Attempted to use nonexistent package ${USED_PACKAGE}")
    ENDIF (NOT ${USED_PACKAGE}_FOUND)

    # Set up the include and link directives
    INCLUDE_DIRECTORIES (${${USED_PACKAGE}_INCLUDE_DIR})
    TARGET_LINK_LIBRARIES (${MY_PROJECT} ${${USED_PACKAGE}_LIBRARIES})

  ENDFOREACH (_package ${${MY_PROJECT}_USE_PACKAGES_PACKAGES})
ENDMACRO (USE_PACKAGES MY_PROJECT)

# ------------------------------------------------------------------------------
# Macro: HAWC_PROJECT
# Sets the name of the project to ARG, and defines various project-level
# variables for python and docs.  The following options are defined:
#
# - DOCS_DIR ......... location of *.rst files for sphinx documentation
# ------------------------------------------------------------------------------
MACRO (HAWC_PROJECT MY_PROJECT)
  PROJECT (${MY_PROJECT} CXX C)
  PARSE_ARGUMENTS (${MY_PROJECT}_ARGS
    "DOCS_DIR"
    "NOT_INSPECTABLE"
    ${ARGN})

  # Set up docs directory
  IF (${MY_PROJECT}_ARGS_DOCS_DIR)
    FILE (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/project-doc-links") 
    EXECUTE_PROCESS (COMMAND ln -fsn ${CMAKE_CURRENT_SOURCE_DIR}/${${MY_PROJECT}_ARGS_DOCS_DIR} ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/project-doc-links/${MY_PROJECT})
  ENDIF (${MY_PROJECT}_ARGS_DOCS_DIR)
ENDMACRO (HAWC_PROJECT MY_PROJECT)

# ------------------------------------------------------------------------------
# Macro: HAWC_ADD_LIBRARY
# Define the sources, headers, and link/include dependencies of a library in
# a local project.  Borrows heavily from the the i3_add_library() macro in
# icetray.  Replaces CMake add_library().  The following options are defined:
#
# - USE_PROJECTS ..... define the projects that this project needs (if any)
# - USE_PACKAGES ..... define any external packages needed by the project
# - HEADERS .......... list of the project header files (*.h, *-inl.h)
# - BUILD_HEADERS .... list of headers that depend on the build (*.h.in files)
# - SOURCES .......... list of definition files (*.cc)
# - CONFIGS .......... configuration data that depend on the build (*.in files)
# - EXAMPLES ......... example driver programs, to be copied to config dir
# - LINKDEF .......... ROOT dictionary linkdef header for the project
# - LINKDEF_INFILES .. list of headers needed by the ROOT dictionary
# - INSTALL_DIR ...... override the default location for installing libraries
# - MODULE ........... set up library as a module plugin loaded with dlopen
# ------------------------------------------------------------------------------
MACRO (HAWC_ADD_LIBRARY MY_LIB)
  PARSE_ARGUMENTS (${MY_LIB}_ARGS
    "USE_PROJECTS;USE_PACKAGES;HEADERS;BUILD_HEADERS;SOURCES;CONFIGS;EXAMPLES;LINKDEF;LINKDEF_INFILES;INSTALL_DIR"
    "NOT_INSPECTABLE;MODULE"
    ${ARGN})

  # Add project include directories 
  INCLUDE_DIRECTORIES (
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/include)
  
  # Set up library sources and headers, including any ROOT dictionaries
  NO_DOTFILE_GLOB (${MY_LIB}_HEADERS ${${MY_LIB}_ARGS_HEADERS})
  NO_DOTFILE_GLOB (${MY_LIB}_BUILD_HEADERS ${${MY_LIB}_ARGS_BUILD_HEADERS})
  NO_DOTFILE_GLOB (${MY_LIB}_SOURCES ${${MY_LIB}_ARGS_SOURCES})

  SET (DICTIONARY_SOURCES)
  IF (${MY_LIB}_ARGS_LINKDEF)
    # Check for sufficient dictionary options
    IF (NOT ${MY_LIB}_ARGS_LINKDEF_INFILES)
      MESSAGE (FATAL_ERROR "LINKDEF option requires LINKDEF_INFILES too!")
    ENDIF (NOT ${MY_LIB}_ARGS_LINKDEF_INFILES)

    # Prepend the project path to each input file
    SET (_my_linkdef ${PROJECT_SOURCE_DIR}/${${MY_LIB}_ARGS_LINKDEF})
    SET (_my_linkdef_infiles)
    FOREACH (_infile ${${MY_LIB}_ARGS_LINKDEF_INFILES})
      LIST (APPEND _my_linkdef_infiles ${PROJECT_SOURCE_DIR}/${_infile})
    ENDFOREACH (_infile ${${MY_LIB}_ARGS_LINKDEF_INFILES})

    LIST (APPEND _my_linkdef_include_dirs ${PROJECT_SOURCE_DIR}/include)

    # Generate the dictionary
    ROOT_GENERATE_DICTIONARY (${MY_LIB}
      LINKDEF ${_my_linkdef}
      INFILES ${_my_linkdef_infiles}
      INCLUDE_DIRS ${_my_linkdef_include_dirs})

    SET (DICTIONARY_SOURCES ${${MY_LIB}_DICT_SOURCE})
  ENDIF (${MY_LIB}_ARGS_LINKDEF)

  # Create as a shared object by default, but also allow for module plugins
  IF (${${MY_LIB}_ARGS_MODULE})
    ADD_LIBRARY (${MY_LIB} MODULE ${${MY_LIB}_SOURCES} ${DICTIONARY_SOURCES})
  ELSE (${${MY_LIB}_ARGS_MODULE})
    ADD_LIBRARY (${MY_LIB} SHARED ${${MY_LIB}_SOURCES} ${DICTIONARY_SOURCES})
  ENDIF (${${MY_LIB}_ARGS_MODULE})

  # Configure any project headers that may depend on the CMake build options
  FOREACH (_build_header ${${MY_LIB}_BUILD_HEADERS})
    STRING (REGEX MATCH ".*/(.*)\\.in" _in_file ${_build_header})
    IF (_in_file)
      STRING (REGEX REPLACE ".*/(.*)\\.in" "\\1" _out_file ${_in_file})
      SET (_out_file "${PROJECT_BINARY_DIR}/include/${PROJECT_NAME}/${_out_file}")
      CONFIGURE_FILE (${_in_file} ${_out_file} @ONLY)
      LIST (APPEND ${MY_LIB}_HEADERS ${_out_file})
    ENDIF (_in_file)
  ENDFOREACH (_build_header ${${MY_LIB}_BUILD_HEADERS})

  # Install headers to /path/to/install/include, preserving nested directories
  SET (_include_regex ".*include/(.*)")
  FOREACH (HEADER_FULLPATH ${${MY_LIB}_HEADERS})
    STRING (REGEX MATCH "${_include_regex}" RELPATH ${HEADER_FULLPATH})
    IF (RELPATH)
      STRING (REGEX REPLACE "${_include_regex}" "\\1" HEADER_RELPATH ${HEADER_FULLPATH})
      GET_FILENAME_COMPONENT (RELPATH ${HEADER_RELPATH} PATH)
      INSTALL (FILES ${HEADER_FULLPATH} DESTINATION "include/${RELPATH}"
              PERMISSIONS OWNER_READ OWNER_WRITE
                          GROUP_READ GROUP_WRITE
                          WORLD_READ)
    ENDIF (RELPATH)
  ENDFOREACH (HEADER_FULLPATH ${${MY_LIB}_HEADERS})

  # Install to a special location if requested; else to /path/to/install/lib
  IF (${MY_LIB}_ARGS_INSTALL_DIR)
    INSTALL (TARGETS ${MY_LIB}
             LIBRARY
             DESTINATION ${${MY_LIB}_ARGS_INSTALL_DIR}
             PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                         GROUP_READ GROUP_WRITE GROUP_EXECUTE
                         WORLD_READ WORLD_EXECUTE)
  ELSE (${MY_LIB}_ARGS_INSTALL_DIR)
    INSTALL (TARGETS ${MY_LIB}
             LIBRARY
             DESTINATION lib
             PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                         GROUP_READ GROUP_WRITE GROUP_EXECUTE
                         WORLD_READ WORLD_EXECUTE)
  ENDIF (${MY_LIB}_ARGS_INSTALL_DIR)

  # Install config files to /path/to/install/share/config
  NO_DOTFILE_GLOB (${MY_LIB}_CONFIGS ${${MY_LIB}_ARGS_CONFIGS})
  SET (_config_files)
  FOREACH (_config ${${MY_LIB}_CONFIGS})
    # Configure *.in files when they are encountered
    STRING (REGEX MATCH ".*/(.*)\\.in" _in_file ${_config})
    IF (_in_file)
      STRING (REGEX REPLACE ".*/(.*)\\.in" "\\1" _out_file ${_in_file})
      SET (_out_file "${CMAKE_CURRENT_BINARY_DIR}/config/${_out_file}")
      CONFIGURE_FILE (${_in_file} ${_out_file} @ONLY)
      LIST (APPEND _config_files ${_out_file})
    # Otherwise, just append the config file to the installation list
    ELSE (_in_file)
      LIST (APPEND _config_files ${_config})
    ENDIF (_in_file)
  ENDFOREACH (_config ${${MY_LIB}_CONFIGS})

  INSTALL (FILES ${_config_files} DESTINATION "${HAWC_SHARE}/config"
           PERMISSIONS OWNER_READ OWNER_WRITE
                       GROUP_READ GROUP_WRITE
                       WORLD_READ)

  # Install example code to /path/to/install/share/hawc/examples/${PROJECT_NAME}
  NO_DOTFILE_GLOB (${MY_LIB}_EXAMPLES ${${MY_LIB}_ARGS_EXAMPLES})
  SET (_example_files)
  FOREACH (_example ${${MY_LIB}_EXAMPLES})
    # Configure *.in files (e.g., makefiles) when they are encountered
    STRING (REGEX MATCH ".*/(.*)\\.in" _in_file ${_example})
    IF (_in_file)
      STRING (REGEX REPLACE ".*/(.*)\\.in" "\\1" _out_file ${_in_file})
      SET (_out_file "${CMAKE_CURRENT_BINARY_DIR}/examples/${_out_file}")
      CONFIGURE_FILE (${_in_file} ${_out_file} @ONLY)
      LIST (APPEND _example_files ${_out_file})
    # Else, just append the example file to the installation list
    ELSE (_in_file)
      LIST (APPEND _example_files ${_example})
    ENDIF (_in_file)
  ENDFOREACH (_example ${${MY_LIB}_EXAMPLES})

  INSTALL (FILES ${_example_files}
    DESTINATION "${HAWC_SHARE}/examples/${PROJECT_NAME}"
    PERMISSIONS OWNER_READ OWNER_WRITE
                GROUP_READ GROUP_WRITE
                WORLD_READ)

  # Add external project dependencies
  USE_PROJECTS (${MY_LIB}
    PROJECTS "${${MY_LIB}_ARGS_USE_PROJECTS}")

  # Add external package dependencies
  USE_PACKAGES (${MY_LIB}
    PACKAGES "${${MY_LIB}_ARGS_USE_PACKAGES}")

  # Set up compiler and linker flags for building against this library
  # Don't do it if the library is actually python binding module
  STRING (REGEX MATCH ".*-pybindings" _is_pybinding ${MY_LIB})
  IF (NOT _is_pybinding)
    IF (NOT HAWC_CPPFLAGS)
      SET (HAWC_CPPFLAGS "-I${HAWC_INCDIR}" PARENT_SCOPE)
    ENDIF (NOT HAWC_CPPFLAGS)

    IF (NOT HAWC_LDFLAGS)
      SET (HAWC_LDFLAGS "-L${HAWC_LIBDIR}" PARENT_SCOPE)
    ENDIF (NOT HAWC_LDFLAGS)
    SET (HAWC_LDFLAGS "${HAWC_LDFLAGS} -l${MY_LIB}" PARENT_SCOPE)

    # Make local copies of library in build tree, handy for unit testing
    GET_PROPERTY (_libloc TARGET ${MY_LIB} PROPERTY LOCATION)
    GET_FILENAME_COMPONENT (_libnam ${_libloc} NAME)

    ADD_CUSTOM_COMMAND (TARGET ${MY_LIB} POST_BUILD
      COMMAND mkdir -p "${CMAKE_BINARY_DIR}/libtest/hawc"
      COMMAND ln -sfn "${_libloc}" "${CMAKE_BINARY_DIR}/libtest/hawc/${_libnam}")
  ENDIF (NOT _is_pybinding)

ENDMACRO (HAWC_ADD_LIBRARY MY_LIB)

# ------------------------------------------------------------------------------
# Macro: HAWC_ADD_PYBINDINGS
# Create a library for python bindings inside a given project with name
# "modulename.so."  Takes the same arguments as HAWC_ADD_LIBRARY.
# ------------------------------------------------------------------------------
MACRO (HAWC_ADD_PYBINDINGS MODULENAME)
  IF (ENABLE_PYTHON_BINDINGS)
    MESSAGE (STATUS "    + Will build python bindings for ${MODULENAME}.")
    HAWC_ADD_LIBRARY(${MODULENAME}-pybindings ${ARGN}
      INSTALL_DIR lib/hawc
      MODULE)

    SET_TARGET_PROPERTIES (${MODULENAME}-pybindings
      PROPERTIES
      PREFIX ""
      OUTPUT_NAME ${MODULENAME}
      DEFINE_SYMBOL HAWC_PYBINDINGS_MODULE)

    # Make local copies of modules in build tree, for unit testing.
    # The modules will be copied to a directory called hawc which will then be
    # put into the (DY)LD_LIBRARY_PATH variable in the test environment.
    GET_PROPERTY (_libloc TARGET ${MODULENAME}-pybindings PROPERTY LOCATION)
    GET_FILENAME_COMPONENT (_libnam ${_libloc} NAME)

    ADD_CUSTOM_COMMAND (TARGET ${MODULENAME}-pybindings POST_BUILD
      COMMAND mkdir -p "${CMAKE_BINARY_DIR}/libtest/hawc"
      COMMAND ln -sfn "${_libloc}" "${CMAKE_BINARY_DIR}/libtest/hawc/${_libnam}")
    
  ELSE (ENABLE_PYTHON_BINDINGS)
    MESSAGE (STATUS "    - Will NOT build python bindings for ${MODULENAME}.")
  ENDIF (ENABLE_PYTHON_BINDINGS)
ENDMACRO (HAWC_ADD_PYBINDINGS MODULENAME)

# ------------------------------------------------------------------------------
# Macro: HAWC_ADD_EXECUTABLE
# Define the sources and library dependencies of a project executable.
# Replaces CMake add_executable().  The following options are defined:
#
# - USE_PROJECTS ..... projects this program needs (if any)
# - USE_PACKAGES ..... external packages this program needs
# - SOURCES .......... driver code
# - NO_PREFIX ........ if omitted, project name is prepended to program name
# ------------------------------------------------------------------------------
MACRO (HAWC_ADD_EXECUTABLE MY_EXE)
  PARSE_ARGUMENTS (${PROJECT_NAME}_${MY_EXE}_ARGS
    "USE_PROJECTS;USE_PACKAGES;SOURCES"
    "NO_PREFIX"
    ${ARGN})

  NO_DOTFILE_GLOB(${PROJECT_NAME}_${MY_EXE}_SOURCES
    ${${PROJECT_NAME}_${MY_EXE}_ARGS_SOURCES})

  # Executable will be named project-exename if NO_PREFIX is false
  IF (${PROJECT_NAME}_${MY_EXE}_ARGS_NO_PREFIX)
    SET (${PROJECT_NAME}_${MY_EXE}_TARGET_NAME ${MY_EXE})
  ELSE (${PROJECT_NAME}_${MY_EXE}_ARGS_NO_PREFIX)
    SET (${PROJECT_NAME}_${MY_EXE}_TARGET_NAME ${PROJECT_NAME}-${MY_EXE})
  ENDIF (${PROJECT_NAME}_${MY_EXE}_ARGS_NO_PREFIX)

  # Add executable dependencies
  ADD_EXECUTABLE (${${PROJECT_NAME}_${MY_EXE}_TARGET_NAME}
    ${${PROJECT_NAME}_${MY_EXE}_SOURCES})

  USE_PROJECTS (${${PROJECT_NAME}_${MY_EXE}_TARGET_NAME}
    PROJECTS "${${PROJECT_NAME}_${MY_EXE}_ARGS_USE_PROJECTS}")

  USE_PACKAGES (${${PROJECT_NAME}_${MY_EXE}_TARGET_NAME}
    PACKAGES "${${PROJECT_NAME}_${MY_EXE}_ARGS_USE_PACKAGES}")

  INSTALL (TARGETS ${${PROJECT_NAME}_${MY_EXE}_TARGET_NAME} DESTINATION bin
           PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                       GROUP_READ GROUP_WRITE GROUP_EXECUTE
                       WORLD_READ WORLD_EXECUTE)

ENDMACRO (HAWC_ADD_EXECUTABLE MY_EXE)

# ------------------------------------------------------------------------------
# Macro: HAWC_ADD_APPLICATION
# Define the sources of a standalone application that will go into the
# $HAWC_SHARE directory
#
# - FILES .......... all source files or makefiels to install to $HAWC_SHARE
# - SCRIPTS ........ scripts (python, bash, etc.) to install to $HAWC_SHARE
# ------------------------------------------------------------------------------
MACRO (HAWC_ADD_APPLICATION MY_APP)
  PARSE_ARGUMENTS (${PROJECT_NAME}_${MY_APP}_ARGS
    "FILES;SCRIPTS"
    "NO_PREFIX"
    ${ARGN})

  SET (APP_NAME ${MY_APP})

  # Expand the list of files in the source tree
  NO_DOTFILE_GLOB(${PROJECT_NAME}_${MY_APP}_FILES
    ${${PROJECT_NAME}_${MY_APP}_ARGS_FILES})

  NO_DOTFILE_GLOB(${PROJECT_NAME}_${MY_APP}_SCRIPTS
    ${${PROJECT_NAME}_${MY_APP}_ARGS_SCRIPTS})

  FOREACH (_file ${${PROJECT_NAME}_${MY_APP}_FILES})
    # Identify .in files that need to be configured by the build
    STRING (REGEX MATCH ".*/(.*)\\.in" _in_file ${_file})
    IF (_in_file)
      STRING (REGEX REPLACE ".*/(.*)\\.in" "\\1" _out_file ${_in_file})
      SET (_out_file "${CMAKE_CURRENT_BINARY_DIR}/configured/${_out_file}")
      CONFIGURE_FILE (${_in_file} ${_out_file} @ONLY)
      LIST (APPEND _app_files ${_out_file})
    # Regular files; just push into source list
    ELSE ()
      LIST (APPEND _app_files ${_file})
    ENDIF ()
  ENDFOREACH ()

  # Configure scripts just like regular files, but store them in a separate
  # list and install with executable permissions
  FOREACH (_file ${${PROJECT_NAME}_${MY_APP}_SCRIPTS})
    # Identify .in files that need to be configured by the build
    STRING (REGEX MATCH ".*/(.*)\\.in" _in_file ${_file})
    IF (_in_file)
      STRING (REGEX REPLACE ".*/(.*)\\.in" "\\1" _out_file ${_in_file})
      SET (_out_file "${CMAKE_CURRENT_BINARY_DIR}/configured/${_out_file}")
      CONFIGURE_FILE (${_in_file} ${_out_file} @ONLY)
      LIST (APPEND _app_scripts ${_out_file})
    # Regular files; just push into source list
    ELSE ()
      LIST (APPEND _app_scripts ${_file})
    ENDIF ()
  ENDFOREACH ()

  # Set up the name of the installation directory:
  # /path/to/install/share/(blah blah blah)
  STRING (REPLACE "${CMAKE_SOURCE_DIR}" "" _shdir "${CMAKE_CURRENT_SOURCE_DIR}")
  SET (OUT_DIR "${HAWC_SHARE}${_shdir}")

  INSTALL (FILES ${_app_files} DESTINATION ${OUT_DIR}
           PERMISSIONS OWNER_READ OWNER_WRITE
                       GROUP_READ GROUP_WRITE
                       WORLD_READ)
  INSTALL (PROGRAMS ${_app_scripts} DESTINATION ${OUT_DIR}
           PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                       GROUP_READ GROUP_WRITE GROUP_EXECUTE
                       WORLD_READ WORLD_EXECUTE)
ENDMACRO ()


# ------------------------------------------------------------------------------
# Macro: HAWC_ADD_TEST
# Add a unit test to be executed with CTest.  The following options are defined:
#
# - USE_PROJECTS ..... projects this program needs (if any)
# - USE_PACKAGES ..... external packages this program needs
# - SOURCES .......... driver code
# - CONFIGS .......... configuration data for tests
# - SCRIPTS .......... python scripts used to test project python bindings
# - NO_PREFIX ........ if omitted, project name is prepended to program name
# ------------------------------------------------------------------------------
MACRO (HAWC_ADD_TEST MY_EXE)
  PARSE_ARGUMENTS (${PROJECT_NAME}_${MY_EXE}_ARGS
    "USE_PROJECTS;USE_PACKAGES;SOURCES;SCRIPTS;CONFIGS"
    "NO_PREFIX"
    ${ARGN})

  NO_DOTFILE_GLOB(${PROJECT_NAME}_${MY_EXE}_SOURCES
    ${${PROJECT_NAME}_${MY_EXE}_ARGS_SOURCES})

  # Executable will be named project-exename if NO_PREFIX is false
  IF (${PROJECT_NAME}_${MY_EXE}_ARGS_NO_PREFIX)
    SET (${PROJECT_NAME}_${MY_EXE}_TARGET_NAME ${MY_EXE})
  ELSE (${PROJECT_NAME}_${MY_EXE}_ARGS_NO_PREFIX)
    SET (${PROJECT_NAME}_${MY_EXE}_TARGET_NAME ${PROJECT_NAME}-${MY_EXE})
  ENDIF (${PROJECT_NAME}_${MY_EXE}_ARGS_NO_PREFIX)

  SET (${PROJECT_NAME}_${MY_EXE}_TARGET_NAME
       "test_${${PROJECT_NAME}_${MY_EXE}_TARGET_NAME}")

  # Install config files to current build directory.  Configure *.in files when
  # they are encountered
  NO_DOTFILE_GLOB (${PROJECT_NAME}_${MY_EXE}_CONFIGS
    ${${PROJECT_NAME}_${MY_EXE}_ARGS_CONFIGS})

  FOREACH (_config ${${PROJECT_NAME}_${MY_EXE}_CONFIGS})
    STRING (REGEX MATCH ".*/(.*)\\.in" _in_file ${_config})
    IF (_in_file)
      STRING (REGEX REPLACE ".*/(.*)\\.in" "\\1" _out_file ${_in_file})
      SET (_out_file "${CMAKE_BINARY_DIR}/libtest/${_out_file}")
      CONFIGURE_FILE (${_in_file} ${_out_file} @ONLY)
    ELSE ()
      # Copy directories recursively to libtest
      IF (IS_DIRECTORY ${_config})
        FILE (COPY ${_config} DESTINATION "${CMAKE_BINARY_DIR}/libtest")
      # Copy individual files to libtest
      ELSE ()
        GET_FILENAME_COMPONENT (_name ${_config} NAME)
        SET (_out_file "${CMAKE_BINARY_DIR}/libtest/${_name}")
        CONFIGURE_FILE (${_config} ${_out_file} COPYONLY)
      ENDIF ()
    ENDIF ()
  ENDFOREACH()

  # Add executable dependencies
  ADD_EXECUTABLE (${${PROJECT_NAME}_${MY_EXE}_TARGET_NAME}
    ${${PROJECT_NAME}_${MY_EXE}_SOURCES})

  SET_TARGET_PROPERTIES (${${PROJECT_NAME}_${MY_EXE}_TARGET_NAME}
    PROPERTIES COMPILE_FLAGS -DTEST_DIR="${CMAKE_BINARY_DIR}/libtest")

  USE_PROJECTS (${${PROJECT_NAME}_${MY_EXE}_TARGET_NAME}
    PROJECTS "${${PROJECT_NAME}_${MY_EXE}_ARGS_USE_PROJECTS}")

  USE_PACKAGES (${${PROJECT_NAME}_${MY_EXE}_TARGET_NAME}
    PACKAGES "${${PROJECT_NAME}_${MY_EXE}_ARGS_USE_PACKAGES}")

  ADD_TEST (${${PROJECT_NAME}_${MY_EXE}_TARGET_NAME}
            ${CMAKE_CURRENT_BUILD_DIR}/${${PROJECT_NAME}_${MY_EXE}_TARGET_NAME}
            --log_level=test_suite)

  SET_PROPERTY (TEST ${${PROJECT_NAME}_${MY_EXE}_TARGET_NAME} PROPERTY
    ENVIRONMENT
    HAWC_SRC=${CMAKE_SOURCE_DIR})

  # Go through the list of test scripts and add individual unit tests for each.
  # Run the tests in the libtest directory of the build tree.  It is necessary
  # to set up the testing environment correctly before proceeding
  NO_DOTFILE_GLOB (${PROJECT_NAME}_${MY_EXE}_SCRIPTS
    ${${PROJECT_NAME}_${MY_EXE}_ARGS_SCRIPTS})
  
  FOREACH (_script ${${PROJECT_NAME}_${MY_EXE}_SCRIPTS})
    GET_FILENAME_COMPONENT (script_name ${_script} NAME)

    ADD_TEST (NAME ${PROJECT_NAME}::${script_name}
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/libtest"
      COMMAND ${PYTHON_EXECUTABLE} ${_script})

    # Set the PYTHONPATH and TEST_DIR variables for this test
    SET_PROPERTY (TEST ${PROJECT_NAME}::${script_name} PROPERTY
      ENVIRONMENT
      PYTHONPATH=${CMAKE_BINARY_DIR}/libtest:$ENV{PYTHONPATH})

    SET_PROPERTY (TEST ${PROJECT_NAME}::${script_name} APPEND PROPERTY
      ENVIRONMENT
      TEST_DIR=${CMAKE_BINARY_DIR}/libtest)

    # Set the (DY)LD_LIBRARY_PATH for the test
    IF (CMAKE_HOST_APPLE)
      SET_PROPERTY (TEST ${PROJECT_NAME}::${script_name} APPEND PROPERTY
        ENVIRONMENT
        DYLD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/libtest/hawc:$ENV{DYLD_LIBRARY_PATH})
    ELSE ()
      SET_PROPERTY (TEST ${PROJECT_NAME}::${script_name} APPEND PROPERTY
        ENVIRONMENT
        LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/libtest/hawc:$ENV{LD_LIBRARY_PATH})
    ENDIF ()
  ENDFOREACH ()

ENDMACRO (HAWC_ADD_TEST MY_EXE)

# ------------------------------------------------------------------------------
# Macro: HAWC_ADD_SCRIPT
# Define the sources of a standalone application that will go into the
# $HAWC_SHARE directory
#
# - SCRIPTS ........ scripts (python, bash, etc.) to install to $HAWC_INSTALL/bin
# ------------------------------------------------------------------------------
MACRO (HAWC_ADD_SCRIPTS MY_SCRIPT)
  PARSE_ARGUMENTS (${PROJECT_NAME}_${MY_SCRIPT}_ARGS
    "FILES;SCRIPTS"
    "NO_PREFIX"
    ${ARGN})

  SET (SCRIPT_NAME ${MY_SCRIPT})




  # Expand the list of files in the source tree
  NO_DOTFILE_GLOB(${PROJECT_NAME}_${MY_SCRIPT}_FILES
    ${${PROJECT_NAME}_${MY_SCRIPT}_ARGS_FILES})


  FOREACH (_file ${${PROJECT_NAME}_${MY_SCRIPT}_FILES})
    # Identify .in files that need to be configured by the build
    STRING (REGEX MATCH ".*/(.*)\\.in" _in_file ${_file})
    IF (_in_file)
      STRING (REGEX REPLACE ".*/(.*)\\.in" "\\1" _out_file ${_in_file})
      SET (_out_file "${CMAKE_CURRENT_BINARY_DIR}/configured/${_out_file}")
      CONFIGURE_FILE (${_in_file} ${_out_file} @ONLY)
      LIST (APPEND _app_files ${_out_file})
    # Regular files; just push into source list
    ELSE ()
      LIST (APPEND _app_files ${_file})
    ENDIF ()
  ENDFOREACH ()

  # Expand the list of scripts in the source tree
  NO_DOTFILE_GLOB(${PROJECT_NAME}_${MY_SCRIPT}_SCRIPTS
    ${${PROJECT_NAME}_${MY_SCRIPT}_ARGS_SCRIPTS})

  # Configure scripts just like regular files, but store them in a separate
  # list and install with executable permissions
  FOREACH (_file ${${PROJECT_NAME}_${MY_SCRIPT}_SCRIPTS})
    # Identify .in files that need to be configured by the build
    STRING (REGEX MATCH ".*/(.*)\\.in" _in_file ${_file})
    IF (_in_file)
      STRING (REGEX REPLACE ".*/(.*)\\.in" "\\1" _out_file ${_in_file})
      SET (_out_file "${CMAKE_CURRENT_BINARY_DIR}/configured/${_out_file}")
      CONFIGURE_FILE (${_in_file} ${_out_file} @ONLY)
      LIST (APPEND _app_scripts ${_out_file})
    # Regular files; just push into source list
    ELSE ()
      LIST (APPEND _app_scripts ${_file})
    ENDIF ()
  ENDFOREACH ()

  # Set up the name of the installation directory:
  # /path/to/install/share/(blah blah blah)
  STRING (REPLACE "${CMAKE_SOURCE_DIR}" "" _shdir "${CMAKE_CURRENT_SOURCE_DIR}")
  SET (OUT_DIR "bin")

  INSTALL (FILES ${_app_files} DESTINATION ${OUT_DIR}
           PERMISSIONS OWNER_READ OWNER_WRITE
                       GROUP_READ GROUP_WRITE
                       WORLD_READ)
  INSTALL (PROGRAMS ${_app_scripts} DESTINATION ${OUT_DIR}
           PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                       GROUP_READ GROUP_WRITE GROUP_EXECUTE
                       WORLD_READ WORLD_EXECUTE)
ENDMACRO (HAWC_ADD_SCRIPTS MY_SCRIPT)

