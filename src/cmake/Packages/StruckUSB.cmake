################################################################################
# Module to find Struck USB headers and libraries                              #
#                                                                              #
# This module defines:                                                         #
#                                                                              #
#   STRUCK_USB_FOUND                                                           #
#   STRUCK_USB_LIBRARIES                                                       #
#   STRUCK_USB_INCLUDE_DIR                                                     #
#   STRUCK_USB_CPPFLAGS                                                        #
#   STRUCK_USB_LDFLAGS                                                         #
################################################################################

SET (STRUCK_USB_FIND_QUIETLY TRUE)
SET (STRUCK_USB_FIND_REQUIRED FALSE)

IF (NOT STRUCK_USB_FOUND)

  SET (STRUCK_USBROOT $ENV{STRUCK_USBROOT})

  IF (NOT STRUCK_USBROOT)
    SET (STRUCK_USBROOT "/usr/local/driver/struck")
  ENDIF (NOT STRUCK_USBROOT)
  
  # Search user environment for headers, then default paths
  FIND_PATH (STRUCK_USB_INCLUDE_DIR sis3150usb_vme_calls.h
    PATHS ${STRUCK_USBROOT}/include
    NO_DEFAULT_PATH)

  FIND_PATH (STRUCK_USB_INCLUDE_DIR sis3150usb_vme_calls.h
    PATHS ${STRUCK_USBROOT}/../include
    NO_DEFAULT_PATH)

  FIND_PATH (STRUCK_USB_INCLUDE_DIR sis3150usb_vme_calls.h)
  GET_FILENAME_COMPONENT (STRUCK_USBROOT ${STRUCK_USB_INCLUDE_DIR} PATH)

  # Search user environment for libraries, then default paths
  FIND_LIBRARY (STRUCK_USB_LIBRARIES NAMES sis3150 sis3150tcl usrp
    PATHS ${STRUCK_USBROOT}/lib
    NO_DEFAULT_PATH)
  FIND_LIBRARY (STRUCK_USB_LIBRARIES NAMES sis3150 sis3150tcl usrp)

  # Set STRUCK_USB_FOUND and error out if sis3150 is not found
  INCLUDE (FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS (STRUCK_USB
    DEFAULT_MSG STRUCK_USB_LIBRARIES STRUCK_USB_INCLUDE_DIR)

  IF (STRUCK_USB_FOUND)
    COLORMSG (HICYAN "Struck USB found")

    # Check to see if functions are prefixed with STRUCK_USB_
#    INCLUDE (CheckLibraryExists)
#    CHECK_LIBRARY_EXISTS (${STRUCK_USB_LIBRARIES} vme_A32D32_read "" STRUCK_USB_LIBRARY_EXISTS)

    # Set flags and print a status message
    SET (STRUCK_USB_CPPFLAGS "-I${STRUCK_USB_INCLUDE_DIR}")
    SET (STRUCK_USB_LDFLAGS "${STRUCK_USB_LIBRARIES}")
    
    MESSAGE (STATUS " * includes: ${STRUCK_USB_INCLUDE_DIR}")
    MESSAGE (STATUS " * libs:     ${STRUCK_USB_LIBRARIES}")
    IF (STRUCK_USB_LIBRARY_EXISTS)
      MESSAGE (STATUS " * confirm:  Library functions exist")
    ENDIF (STRUCK_USB_LIBRARY_EXISTS)
  ELSE (STRUCK_USB_FOUND)
    MESSAGE (STATUS "Struck USB not found")
  ENDIF (STRUCK_USB_FOUND)

ENDIF (NOT STRUCK_USB_FOUND)

