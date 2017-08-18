################################################################################
# CHECK_LANGUAGE_WORKS
# Check for the existence of a compiler for a particular language and return
# the result in the variable language_works.
#
# E.g., to check that Fortran programs can be compiled use
#`
# CHECK_LANGUAGE_WORKS (Fortran Fortran_LANGUAGE_WORKS)
# IF (Fortran_LANGUAGE_WORKS)
#   ... do stuff
# ENDIF (Fortran_LANGUAGE_WORKS)
# -----------------------------------------------------------------------------
# Function copied from
# https://github.com/mousebird/eigen/blob/master/cmake/language_support.cmake
################################################################################

FUNCTION (CHECK_LANGUAGE_WORKS language language_works)
  #message("DEBUG: language = ${language}")
  cmake_minimum_required(VERSION 2.6.0)
  IF ( ${CMAKE_VERSION} VERSION_GREATER 2.8.8 )
    enable_language(${language} OPTIONAL)
    IF (DEFINED CMAKE_${language}_COMPILER_WORKS AND CMAKE_${language}_COMPILER_WORKS )
      SET (${language_works} ON PARENT_SCOPE)
    ELSE(DEFINED CMAKE_${language}_COMPILER_WORKS AND CMAKE_${language}_COMPILER_WORKS )
      SET (${language_works} OFF PARENT_SCOPE)
    ENDIF(DEFINED CMAKE_${language}_COMPILER_WORKS AND CMAKE_${language}_COMPILER_WORKS )

  ELSE ( ${CMAKE_VERSION} VERSION_GREATER 2.8.8 )
    SET (text
      "project(test NONE)
      cmake_minimum_required(VERSION 2.6.0)
      enable_language(${language} )
    ")
    FILE (REMOVE_RECURSE ${CMAKE_BINARY_DIR}/language_tests/${language})
    FILE (MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/language_tests/${language})
    FILE (WRITE ${CMAKE_BINARY_DIR}/language_tests/${language}/CMakeLists.txt
      ${text})
    EXECUTE_PROCESS(
      COMMAND ${CMAKE_COMMAND} .
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/language_tests/${language}
      RESULT_VARIABLE return_code
      OUTPUT_QUIET
      ERROR_QUIET
      )

    IF (return_code EQUAL 0)
      # Second run
      EXECUTE_PROCESS (
        COMMAND ${CMAKE_COMMAND} .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/language_tests/${language}
        RESULT_VARIABLE return_code
        OUTPUT_QUIET
        ERROR_QUIET
        )
      IF (return_code EQUAL 0)
        SET (${language_works} ON PARENT_SCOPE)
      ELSE (return_code EQUAL 0)
        SET (${language_works} OFF PARENT_SCOPE)
      ENDIF (return_code EQUAL 0)
    ELSE (return_code EQUAL 0)
      SET (${language_works} OFF PARENT_SCOPE)
    ENDIF (return_code EQUAL 0)
  ENDIF ( ${CMAKE_VERSION} VERSION_GREATER 2.8.8 )
ENDFUNCTION (CHECK_LANGUAGE_WORKS)

