# CMake python compilation hacks:

# Go to the installation directory and recursively compile python byte code
EXECUTE_PROCESS (COMMAND python -mcompileall ${CMAKE_INSTALL_PREFIX}/lib)

# List the byte code created by the previous command
FILE (GLOB_RECURSE HAWC_PYTHON_BYTECODE ${CMAKE_INSTALL_PREFIX}/*.pyc)
EXECUTE_PROCESS (COMMAND chmod g+w ${HAWC_PYTHON_BYTECODE})

# Append all pyc files to the installation manifest.  This ensures that they
# can be uninstalled later
LIST (APPEND CMAKE_INSTALL_MANIFEST_FILES ${HAWC_PYTHON_BYTECODE})

