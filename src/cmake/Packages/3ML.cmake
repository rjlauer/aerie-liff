################################################################################
# Module to find the cJSON headers and libraries                               #
#                                                                              #
# This module defines:                                                         #
#                                                                              #
#   3ML_FOUND                                                              #
#   HAVE_3ML                                                               #
################################################################################

SET (3ML_FIND_QUIETLY TRUE)
SET (3ML_FIND_REQUIRED FALSE)

IF (NOT 3ML_FOUND)

  EXECUTE_PROCESS (COMMAND python -c "import threeML"
    RESULT_VARIABLE _result
    OUTPUT_QUIET ERROR_QUIET)

  IF (_result)
    MESSAGE (STATUS "3ML not found")
  ELSE ()
    SET (3ML_FOUND TRUE)
    SET (HAVE_3ML TRUE)
    ADD_DEFINITIONS (-DHAVE_3ML)
    COLORMSG (HICYAN "3ML found")
  ENDIF ()

ENDIF (NOT 3ML_FOUND)

