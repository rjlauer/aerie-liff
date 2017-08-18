
# ------------------------------------------------------------------------------
# Macro REPOSITORY_REVISION
#
# Extract the revision ID of the code in the software repository.  The body is
# set up to use subversion and its tools, but we may not use svn forever, so it
# has the more awkward (but portable) name "REPOSITORY_REVISION"
# ------------------------------------------------------------------------------
MACRO (REPOSITORY_REVISION srcpath)

  EXECUTE_PROCESS (COMMAND svn info --non-interactive ${srcpath}
                   OUTPUT_VARIABLE _svn_info
                   ERROR_QUIET)

  # Extract version data from "svn info --non-interactive" run on source tree:
  IF (_svn_info)
    STRING (REGEX MATCH "Revision: [0-9]+" _svn_revision ${_svn_info})
    IF (_svn_revision)
      STRING (REPLACE "Revision: " "" _rev_number ${_svn_revision})
      IF (_rev_number)
        SET (SOFTWARE_REPOSITORY_REVISION ${_rev_number}
             CACHE STRING "Repository revision number" FORCE)
      ENDIF (_rev_number)
    ENDIF (_svn_revision)
  # If "svn info --non-interactive" failed in the source tree, try the remote repository as a
  # last resort:
  ELSE (_svn_info)
    # Set the remote URL
    SET (_prfx "http://private.hawc-observatory.org/svn/hawc/workspaces/aerie")
    IF ("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
      SET (_url "${_prfx}/trunk")
    ELSE ()
      SET (_url "${_prfx}/branches/v${SOFTWARE_MAJOR_VERSION}-${SOFTWARE_MINOR_VERSION}")
    ENDIF ()

    # Run "svn info --non-interactive" on the remote URL and extract the last changed revision
    EXECUTE_PROCESS (COMMAND svn info --non-interactive ${_url}
                     OUTPUT_VARIABLE _svn_info
                     ERROR_QUIET)
    IF (_svn_info)
      STRING (REGEX MATCH "Last Changed Rev: [0-9]+" _svn_revision ${_svn_info})
      IF (_svn_revision)
        STRING (REPLACE "Last Changed Rev: " "" _rev_number ${_svn_revision})
        IF (_rev_number)
          SET (SOFTWARE_REPOSITORY_REVISION ${_rev_number}
               CACHE STRING "Repository revision number" FORCE)
        ENDIF (_rev_number)
      ENDIF (_svn_revision)
    ENDIF (_svn_info)
  ENDIF (_svn_info)

  # In case of total failure, just use a dummy string
  IF (NOT SOFTWARE_REPOSITORY_REVISION)
    IF ("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
      SET (SOFTWARE_REPOSITORY_REVISION "unknown"
           CACHE STRING "Repository revision number" FORCE)
    ELSE ()
      SET (SOFTWARE_REPOSITORY_REVISION "${CMAKE_BUILD_TYPE}"
           CACHE STRING "Repository revision number" FORCE)
    ENDIF ()
  ENDIF (NOT SOFTWARE_REPOSITORY_REVISION)
ENDMACRO (REPOSITORY_REVISION)

