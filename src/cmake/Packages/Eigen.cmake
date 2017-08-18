################################################################################
# Module to find Eigen                                                         #
#                                                                              #
#   EIGEN_FOUND                                                                # 
#   EIGEN_CPPFLAGS                                                             #
#   EIGEN_INCLUDE_DIR                                                          #
################################################################################

IF (NOT EIGEN_FOUND)

   IF (EXISTS "$ENV{EIGENROOT}/Eigen")
      SET (EIGEN_FOUND TRUE)
      COLORMSG (HICYAN "Eigen found:")
      SET (EIGEN_CPPFLAGS "-I$ENV{EIGENROOT}")
      SET (EIGEN_INCLUDE_DIR "$ENV{EIGENROOT}")
      MESSAGE (STATUS "  * includes: $ENV{EIGENROOT}")
      ADD_DEFINITIONS ("-DEIGEN_FOUND")  
      ADD_DEFINITIONS ("-DEIGEN_DONT_PARALLELIZE")
   ELSE()
      MESSAGE(STATUS "Eigen not found")
   ENDIF()

ENDIF (NOT EIGEN_FOUND)

