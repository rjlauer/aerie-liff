################################################################################
# Module to find Root                                                          #
#                                                                              #
# This sets the following variables:                                           #
#   - ROOT_FOUND                                                               #
#   - ROOT_LIBRARIES                                                           #
#   - ROOT_GUI_LIBRARIES                                                       #
#   - ROOT_INCLUDE_DIR                                                         #
#   - ROOT_BIN_DIR                                                             #
#   - ROOTSYS                                                                  #
#   - ROOTCINT                                                                 #
#   - ROOT_CPPFLAGS                                                            #
#   - ROOT_LDFLAGS                                                             #
#   - ROOT_VERSION                                                             #
#                                                                              #
# And these might be needed as well... we'll see:                              #
#   - ROOT_LIBS                                                                #
#   - ROOT_GLIBS                                                               #
#   - ROOT_LIB_DIR                                                             #
################################################################################

# If root was found already, don't find it again.
# This works because ROOT_FOUND is not cached
IF (NOT ROOT_FOUND)

# Minimum ROOT requirements.
SET (ROOT_MIN_REQUIRED_VERSION "5.18.00")

# First search in only one path; then try default paths
FIND_PATH (ROOT_BIN_DIR root-config PATHS $ENV{ROOTSYS}/bin NO_DEFAULT_PATH)
FIND_PATH (ROOT_BIN_DIR root-config)

GET_FILENAME_COMPONENT (ROOTSYS ${ROOT_BIN_DIR} PATH)

IF (NOT ENV{ROOTSYS})
  SET (ENV{ROOTSYS} ${ROOTSYS})
ENDIF (NOT ENV{ROOTSYS})

IF (ROOTSYS)
  # ----------------------------------------------------------------------------
  # Get ROOT version, re-express in form XX.YY.ZZ, compare to requirements.
  # ----------------------------------------------------------------------------
  EXECUTE_PROCESS (COMMAND ${ROOT_BIN_DIR}/root-config --version
    OUTPUT_VARIABLE ROOT_VERSION
  )
  STRING (REGEX REPLACE "[ \t\r\n]+" "" ROOT_VERSION ${ROOT_VERSION})
  STRING (REGEX REPLACE "/" "." ROOT_VERSION ${ROOT_VERSION})

  IF (${ROOT_VERSION} VERSION_GREATER ${ROOT_MIN_REQUIRED_VERSION})
    # --------------------------------------------------------------------------
    # Set ROOT compilation flags.
    # --------------------------------------------------------------------------
    SET (ROOT_FOUND TRUE)
    SET (HAVE_ROOT 1)

    EXECUTE_PROCESS (COMMAND ${ROOT_BIN_DIR}/root-config --libdir
      OUTPUT_VARIABLE _ROOT_LIB_DIR)
    STRING (REGEX REPLACE "[ \t\r\n]+" "" ROOT_LIB_DIR ${_ROOT_LIB_DIR})

    EXECUTE_PROCESS (COMMAND ${ROOT_BIN_DIR}/root-config --noldflags --glibs
      OUTPUT_VARIABLE ROOT_LIBS)
    STRING (REGEX REPLACE "[ \t\r\n]+" " " ROOT_LIBS ${ROOT_LIBS})

    EXECUTE_PROCESS (COMMAND ${ROOT_BIN_DIR}/root-config --glibs
      OUTPUT_VARIABLE ROOT_LDFLAGS)
    STRING (REGEX REPLACE "[ \t\r\n]+" " " ROOT_LDFLAGS ${ROOT_LDFLAGS})

    EXECUTE_PROCESS (COMMAND ${ROOT_BIN_DIR}/root-config --glibs
      OUTPUT_VARIABLE ROOT_GLIBS)
    STRING (REGEX REPLACE "[ \t\r\n]+" "" ROOT_GLIBS ${ROOT_GLIBS})

    EXECUTE_PROCESS (COMMAND ${ROOT_BIN_DIR}/root-config --incdir
      OUTPUT_VARIABLE ROOT_INCLUDE_DIR)
    STRING (REGEX REPLACE "[ \t\r\n]+" "" ROOT_INCLUDE_DIR ${ROOT_INCLUDE_DIR})

    EXECUTE_PROCESS (COMMAND ${ROOT_BIN_DIR}/root-config --cflags
      OUTPUT_VARIABLE ROOT_CPPFLAGS)
    STRING (REGEX REPLACE "[ \t\r\n]+" " " ROOT_CPPFLAGS ${ROOT_CPPFLAGS})

    # Check for TMVA library
    EXECUTE_PROCESS (COMMAND ${ROOT_BIN_DIR}/root-config --has-tmva
      OUTPUT_VARIABLE _ROOT_HAS_TMVA)
    STRING (REGEX REPLACE "[ \t\r\n]+" "" _ROOT_HAS_TMVA ${_ROOT_HAS_TMVA})

    IF (${_ROOT_HAS_TMVA} STREQUAL "yes")
      SET (ROOT_LIBS "${ROOT_LIBS} -lTMVA")
      SET (ROOT_LDFLAGS "${ROOT_LDFLAGS} -lTMVA")
      SET (ROOT_GLIBS "${ROOT_GLIBS} -lTMVA")
    ELSE ()
      MESSAGE (STATUS "TMVA ROOT library not found.")
    ENDIF ()

    SET (ROOTCINT ${ROOT_BIN_DIR}/rootcint)

    # --------------------------------------------------------------------------
    # Find ROOT libraries.
    # --------------------------------------------------------------------------
    STRING (REGEX REPLACE "-l" " " CRAP ${ROOT_LIBS})
    STRING (REGEX REPLACE "-" " " CRAP ${CRAP})
    SEPARATE_ARGUMENTS (CRAP)
    FOREACH (_file ${CRAP})
      FIND_LIBRARY (_LIBY${_file} ${_file} ${ROOT_LIB_DIR} NO_DEFAULT_PATH)
      IF (_LIBY${_file})
        LIST (APPEND ROOT_LIBRARIES ${_LIBY${_file}})
      ENDIF (_LIBY${_file})
      SET (_LIBY${_file} ${_LIBY${_file}} CACHE INTERNAL "" FORCE)
    ENDFOREACH (_file)

    STRING (REGEX REPLACE "-l" " " CRAP ${ROOT_GLIBS})
    STRING (REGEX REPLACE "-" " " CRAP ${CRAP})
    SEPARATE_ARGUMENTS (CRAP)
    FOREACH (_file ${CRAP})
      FIND_LIBRARY (_LIBY${_file} ${_file} ${ROOT_LIB_DIR} NO_DEFAULT_PATH)
      IF (_LIBY${_file})
        LIST (APPEND ROOT_GUI_LIBRARIES ${_LIBY${_file}})
      ENDIF (_LIBY${_file})
      SET (_LIBY${_file} ${_LIBY${_file}} CACHE INTERNAL "" FORCE)
    ENDFOREACH (_file)

    FIND_LIBRARY (_ROOT_HP_LIBRARY HistPainter ${ROOT_LIB_DIR} NO_DEFAULT_PATH)
    IF (_ROOT_HP_LIBRARY)
      LIST (APPEND ROOT_LIBRARIES ${_ROOT_HP_LIBRARY})
      LIST (APPEND ROOT_GUI_LIBRARIES ${_ROOT_HP_LIBRARY})
    ENDIF (_ROOT_HP_LIBRARY)

    FIND_LIBRARY (_ROOT_GQT_LIBRARY GQt ${ROOT_LIB_DIR} NO_DEFAULT_PATH)
    FIND_LIBRARY (_ROOT_QTROOT_LIBRARY QtRoot ${ROOT_LIB_DIR} NO_DEFAULT_PATH)
    IF (_ROOT_GQT_LIBRARY AND _ROOT_QTROOT_LIBRARY)
      SET (ROOT_QTROOT_LIBRARIES ${_ROOT_GQT_LIBRARY} ${_ROOT_QTROOT_LIBRARY} ${ROOT_GUI_LIBRARIES})
    ENDIF (_ROOT_GQT_LIBRARY AND _ROOT_QTROOT_LIBRARY)

    # Note: some platforms require the system pthread to be added explicitly
    FIND_LIBRARY (PTHREAD_LIBRARY pthread)
    LIST (APPEND ROOT_LIBRARIES ${PTHREAD_LIBRARY})
    LIST (APPEND ROOT_GUI_LIBRARIES ${PTHREAD_LIBRARY})

  ELSE (${ROOT_VERSION} VERSION_GREATER ${ROOT_MIN_REQUIRED_VERSION})

    SET (ROOT_FOUND FALSE)
    SET (HAVE_ROOT 0)

    IF (NOT Root_FIND_QUIETLY)
      MESSAGE (STATUS
        "Compatible version of ROOT not found. "
        "Minimum required version: ${ROOT_MIN_REQUIRED_VERSION}")
    ENDIF (NOT Root_FIND_QUIETLY)

  ENDIF (${ROOT_VERSION} VERSION_GREATER ${ROOT_MIN_REQUIRED_VERSION})

ENDIF (ROOTSYS)

SET (_DIRECTORIES)

IF (NOT DL_LIBRARY)
  FIND_LIBRARY (DL_LIBRARY dl)
  MARK_AS_ADVANCED (DL_LIBRARY)
ENDIF (NOT DL_LIBRARY)
SET (ROOT_LIBRARIES ${ROOT_LIBRARIES} ${DL_LIBRARY})

IF (ROOT_FOUND)
  #add minuit
  FIND_LIBRARY (ROOT_MINUIT_LIB Minuit ${ROOT_LIB_DIR} NO_DEFAULT_PATH)

  IF (ROOT_MINUIT_LIB)
    SET (ROOT_LDFLAGS "${ROOT_LDFLAGS} -lMinuit")
    SET (ROOT_MINUIT_LIB ${ROOT_MINUIT_LIB} CACHE INTERNAL "" FORCE)
  ELSE (ROOT_MINUIT_LIB)
    MESSAGE(FATAL_ERROR "Could not find TMinuit library in ${ROOT_LIB_DIR}")
  ENDIF (ROOT_MINUIT_LIB)
  LIST (APPEND ROOT_LIBRARIES ${ROOT_MINUIT_LIB})

#  LINK_DIRECTORIES (${ROOT_LIBRARY_DIR})
  INCLUDE_DIRECTORIES (${ROOT_INCLUDE_DIR})

  IF (NOT Root_FIND_QUIETLY)
    COLORMSG (HICYAN "Root version: ${ROOT_VERSION}")
    MESSAGE (STATUS "  * binary:   ${ROOT_BIN_DIR}/root")
    MESSAGE (STATUS "  * includes: ${ROOT_INCLUDE_DIR}")
    MESSAGE (STATUS "  * libs:     ${ROOT_LIB_DIR}")
  ENDIF (NOT Root_FIND_QUIETLY)
ELSE (ROOT_FOUND)
  IF (Root_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find Root")
  else (Root_FIND_REQUIRED)
    if (NOT Root_FIND_QUIETLY)
      MESSAGE (STATUS "Could not find Root")
    endif (NOT Root_FIND_QUIETLY)
  ENDIF (Root_FIND_REQUIRED)
ENDIF (ROOT_FOUND)

# ------------------------------------------------------------------------------
# Macro: ROOT_GENERATE_DICTIONARY
#
# Creates rules for a dictionary.  If the LinkDef file is called SomeLinkDef.h,
# this will create SomeDictionary.cc and SomeDictionary.h.  Defined variables:
#
#   Some_DICT_SOURCE
#   Some_DICT_HEADER
#   Some_INFILES
#   Some_INCLUDE_DIRS
#   Some_VERBOSE
# ------------------------------------------------------------------------------
MACRO (ROOT_GENERATE_DICTIONARY MYDICT)
  PARSE_ARGUMENTS (${MYDICT}
    "LINKDEF;INFILES;INCLUDE_DIRS;DEFINE_FLAGS"
    "VERBOSE"
    ${ARGN})

  SET (_INCLUDE_DIRS)
  SET (${MYDICT}_DICT_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/${MYDICT}Dictionary.cc)
  SET (${MYDICT}_DICT_HEADER ${CMAKE_CURRENT_BINARY_DIR}/${MYDICT}Dictionary.h)

  # Set up the list of includes
  FOREACH (_current_FILE ${${MYDICT}_INCLUDE_DIRS} ${_DIR_INCLUDES})
    SET (_INCLUDE_DIRS ${_INCLUDE_DIRS} -I${_current_FILE})
  ENDFOREACH (_current_FILE ${${MYDICT}_INCLUDE_DIRS})

  # Set up the list of compiler definitions
  FOREACH (_current_DEF ${${MYDICT}_DEFINE_FLAGS} ${_DEF_FLAGS})
    SET (_DEFINE_FLAGS ${_DEFINE_FLAGS} -D${_current_DEF})
  ENDFOREACH (_current_DEF ${${MYDICT}_DEFINE_FLAGS})

  # Set up the call to rootcint
  IF (${MYDICT}_VERBOSE)
    MESSAGE (STATUS "Root dictionary:\n   ${MYDICT}_INFILES: ${${MYDICT}_INFILES}")
    MESSAGE ("   OutFILEs: ${${MYDICT}_DICT_SOURCE} ${${MYDICT}_DICT_HEADER}")
    MESSAGE ("   LINKDEF: ${${MYDICT}_LINKDEF}")
  ENDIF (${MYDICT}_VERBOSE)

  ADD_CUSTOM_COMMAND (OUTPUT ${${MYDICT}_DICT_SOURCE} ${${MYDICT}_DICT_HEADER}
    COMMAND ${ROOTCINT} -f ${${MYDICT}_DICT_SOURCE} -c -p ${_INCLUDE_DIRS} ${_DEFINE_FLAGS} ${${MYDICT}_INFILES} ${${MYDICT}_LINKDEF}
    DEPENDS ${${MYDICT}_INFILES})

ENDMACRO (ROOT_GENERATE_DICTIONARY)
ENDIF (NOT ROOT_FOUND)

