################################################################################
# Module to find CAEN PCI headers and libraries                                #
#                                                                              #
# This module defines:                                                         #
#                                                                              #
#   CAEN_PCI_FOUND                                                             #
#   CAEN_PCI_LIBRARIES                                                         #
#   CAEN_PCI_INCLUDE_DIR                                                       #
#   CAEN_PCI_NEED_PREFIX is not evaluated                                      #
#   CAEN_PCI_CPPFLAGS                                                          #
#   CAEN_PCI_LDFLAGS                                                           #
################################################################################

SET (CAEN_PCI_FIND_QUIETLY TRUE)
SET (CAEN_PCI_FIND_REQUIRED FALSE)

IF (NOT CAEN_PCI_FOUND)

  SET (CAEN_PCIROOT $ENV{CAEN_PCIROOT})

  IF (NOT CAEN_PCIROOT)
    SET (CAEN_PCIROOT "/usr/local/driver/caen")
  ENDIF (NOT CAEN_PCIROOT)

  # Search user environment for headers, then default paths
  FIND_PATH (CAEN_PCI_INCLUDE_DIR CAENVMElib.h
    PATHS ${CAEN_PCIROOT}/include
    NO_DEFAULT_PATH)

  FIND_PATH (CAEN_PCI_INCLUDE_DIR CAENVMEtypes.h
    PATHS ${CAEN_PCIROOT}/../include
    NO_DEFAULT_PATH)

  FIND_PATH (CAEN_PCI_INCLUDE_DIR CAENVMElib.h)
  GET_FILENAME_COMPONENT (CAEN_PCIROOT ${CAEN_PCI_INCLUDE_DIR} PATH)

  # Search user environment for libraries, then default paths
  FIND_LIBRARY (CAEN_PCI_LIBRARIES NAMES libCAENVME.so
    PATHS ${CAEN_PCIROOT}/lib
    NO_DEFAULT_PATH)

  FIND_LIBRARY (CAEN_PCI_LIBRARIES NAMES libCAENVME.so)

  # Set CAEN_PCI_FOUND and error out if libCAENVME is not found
  INCLUDE (FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS (CAEN_PCI
    DEFAULT_MSG CAEN_PCI_LIBRARIES CAEN_PCI_INCLUDE_DIR)

  IF (CAEN_PCI_FOUND)
    COLORMSG (HICYAN "CAEN PCI found")

    # Check to see if functions are prefixed with CAEN_PCI_
#    INCLUDE (CheckLibraryExists)
#    CHECK_LIBRARY_EXISTS (${CAEN_PCI_LIBRARIES} CAENVME_DeviceReset "" CAEN_PCI_LIBRARY_EXISTS)

    # Set flags and print a status message
    SET (CAEN_PCI_CPPFLAGS "-I${CAEN_PCI_INCLUDE_DIR}")
    SET (CAEN_PCI_LDFLAGS "${CAEN_PCI_LIBRARIES}")
    
    MESSAGE (STATUS " * includes: ${CAEN_PCI_INCLUDE_DIR}")
    MESSAGE (STATUS " * libs:     ${CAEN_PCI_LIBRARIES}")
    IF (CAEN_PCI_LIBRARY_EXISTS)
      MESSAGE (STATUS " * confirm:  Library functions exist")
    ENDIF (CAEN_PCI_LIBRARY_EXISTS)
  ELSE (CAEN_PCI_FOUND)
    MESSAGE (STATUS "CAEN PCI not found")
  ENDIF (CAEN_PCI_FOUND)

ENDIF (NOT CAEN_PCI_FOUND)

