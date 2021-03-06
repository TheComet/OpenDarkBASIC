include (GNUInstallDirs)

project ("odbc"
    LANGUAGES CXX
    VERSION 0.0.1)

# These may not exist
file (MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/src")
file (MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/include/odb-cli")

add_subdirectory ("argdefgen")
argdefgen_target (odbc_argdef
    "${PROJECT_SOURCE_DIR}/src/Actions.argdef"
    "${PROJECT_BINARY_DIR}/src/Actions.argdef.cpp"
    DEFINES_FILE "${PROJECT_BINARY_DIR}/include/odb-cli/Actions.argdef.hpp")

###############################################################################
# Executable
###############################################################################

add_executable (odbc
    ${argdefgen_odbc_argdef_OUTPUTS}
    "src/AST.cpp"
    "src/Banner.cpp"
    "src/BuildInfo.cpp"
    "src/Codegen.cpp"
    "src/Commands.cpp"
    "src/Log.cpp"
    "src/SDK.cpp"
    "src/Warnings.cpp"
    "src/main.cpp")
target_include_directories (odbc
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)
target_link_libraries (odbc
    PRIVATE
        odb-compiler)
set_target_properties (odbc
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${ODB_RUNTIME_DIR}
        INSTALL_RPATH ${CMAKE_INSTALL_LIBDIR})

###############################################################################
# Installation
###############################################################################

install (
    TARGETS odbc
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
