################################################################################
# Module to find Wiener VME USB headers and libraries                          #
#                                                                              #
# This module defines:                                                         #
#                                                                              #
#   WIENER_USB_FOUND                                                           #
#   WIENER_USB_LIBRARIES                                                       #
#   WIENER_USB_INCLUDE_DIR                                                     #
#   WIENER_USB_NEED_PREFIX is not evaluated                                    #
#   WIENER_USB_CPPFLAGS                                                        #
#   WIENER_USB_LDFLAGS                                                         #
################################################################################

SET (WIENER_USB_FIND_QUIETLY TRUE)
SET (WIENER_USB_FIND_REQUIRED FALSE)

IF (NOT WIENER_USB_FOUND)

  SET (WIENER_USBROOT $ENV{WIENER_USBROOT})

  IF (NOT WIENER_USBROOT)
    SET (WIENER_USBROOT "/usr/local/driver/wiener")
  ENDIF (NOT WIENER_USBROOT)

  # Search user environment for headers, then default paths
  FIND_PATH (WIENER_USB_INCLUDE_DIR libxxusb.h
    PATHS ${WIENER_USBROOT}/include
    NO_DEFAULT_PATH)

  FIND_PATH (WIENER_USB_INCLUDE_DIR libxxusb.h)
  GET_FILENAME_COMPONENT (WIENER_USBROOT ${WIENER_USB_INCLUDE_DIR} PATH)

  # Search user environment for libraries, then default paths
  FIND_LIBRARY (WIENER_USB_LIBRARIES NAMES libxx_usb.so
    PATHS ${WIENER_USBROOT}/lib
    NO_DEFAULT_PATH)

  FIND_LIBRARY (WIENER_USB_LIBRARIES NAMES libxx_usb.so)

  # Set WIENER_USB_FOUND and error out if libxx_usb.so is not found
  INCLUDE (FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS (WIENER_USB
    DEFAULT_MSG WIENER_USB_LIBRARIES WIENER_USB_INCLUDE_DIR)

  IF (WIENER_USB_FOUND)
    COLORMSG (HICYAN "WIENER USB found")

    # Check to see if functions are prefixed with WIENER_USB_
#    INCLUDE (CheckLibraryExists)
#    CHECK_LIBRARY_EXISTS (${WIENER_USB_LIBRARIES} CAENVME_DeviceReset "" WIENER_USB_LIBRARY_EXISTS)

    # Set flags and print a status message
    SET (WIENER_USB_CPPFLAGS "-I${WIENER_USB_INCLUDE_DIR}")
    SET (WIENER_USB_LDFLAGS "${WIENER_USB_LIBRARIES}")
    
    MESSAGE (STATUS " * includes: ${WIENER_USB_INCLUDE_DIR}")
    MESSAGE (STATUS " * libs:     ${WIENER_USB_LIBRARIES}")
    IF (WIENER_USB_LIBRARY_EXISTS)
      MESSAGE (STATUS " * confirm:  Library functions exist")
    ENDIF (WIENER_USB_LIBRARY_EXISTS)
  ELSE (WIENER_USB_FOUND)
    MESSAGE (STATUS "WIENER USB not found")
  ENDIF (WIENER_USB_FOUND)

ENDIF (NOT WIENER_USB_FOUND)

