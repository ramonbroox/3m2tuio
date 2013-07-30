# Find LIBUSB includes and library
#
# This module defines
#  LIBUSB_INCLUDE_DIR
#  LIBUSB_LIBRARIES, the libraries to link against to use LIBUSB.
#  LIBUSB_FOUND, If false, do not try to use LIBUSB
# Redistribution and use is allowed according to the terms of the BSD license.
#
# Copyright 2010 Aleix Pol Gonzalez <aleixpol@kde.org>

IF (LIBUSB_LIBRARIES AND LIBUSB_INCLUDE_DIR)
    SET(LIBUSB_FIND_QUIETLY TRUE) # Already in cache, be silent
ENDIF (LIBUSB_LIBRARIES AND LIBUSB_INCLUDE_DIR)

MESSAGE(STATUS "Looking for libusb-1.0")

find_path(LIBUSB_INCLUDE_DIR libusb.h PATH_SUFFIXES libusb-1.0)

find_library(LIBUSB_LIBRARIES NAMES usb-1.0)
mark_as_advanced(LIBUSB_LIBRARIES)

IF (LIBUSB_INCLUDE_DIR AND LIBUSB_LIBRARIES)
    SET(LIBUSB_FOUND TRUE)
ENDIF (LIBUSB_INCLUDE_DIR AND LIBUSB_LIBRARIES)

IF (LIBUSB_FOUND)
   IF (NOT LIBUSB_FIND_QUIETLY)
      MESSAGE(STATUS "Found libusb-1.0")
      MESSAGE(STATUS "  libraries : ${LIBUSB_LIBRARIES}")
      MESSAGE(STATUS "  includes  : ${LIBUSB_INCLUDE_DIR}")
   ENDIF (NOT LIBUSB_FIND_QUIETLY)
ELSE (LIBUSB_FOUND)
    IF (LIBUSB_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find libusb-1.0")
    ENDIF (LIBUSB_FIND_REQUIRED)
ENDIF (LIBUSB_FOUND)

