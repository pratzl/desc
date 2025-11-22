# Installation configuration for graph3 library
# Configures installation paths, exports, and package config files

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Installation directories
set(GRAPH3_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR})
set(GRAPH3_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/graph3)

# Install headers
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION ${GRAPH3_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

# Install targets
install(TARGETS graph3
    EXPORT graph3-targets
    INCLUDES DESTINATION ${GRAPH3_INSTALL_INCLUDEDIR}
)

# Install export set
install(EXPORT graph3-targets
    FILE graph3-targets.cmake
    NAMESPACE graph::
    DESTINATION ${GRAPH3_INSTALL_CMAKEDIR}
)

# Create package config file
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/graph3-config.cmake.in
    ${PROJECT_BINARY_DIR}/graph3-config.cmake
    INSTALL_DESTINATION ${GRAPH3_INSTALL_CMAKEDIR}
)

# Create version file
write_basic_package_version_file(
    ${PROJECT_BINARY_DIR}/graph3-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install config files
install(FILES
    ${PROJECT_BINARY_DIR}/graph3-config.cmake
    ${PROJECT_BINARY_DIR}/graph3-config-version.cmake
    DESTINATION ${GRAPH3_INSTALL_CMAKEDIR}
)

message(STATUS "Installation configured:")
message(STATUS "  Headers: ${CMAKE_INSTALL_PREFIX}/${GRAPH3_INSTALL_INCLUDEDIR}")
message(STATUS "  CMake config: ${CMAKE_INSTALL_PREFIX}/${GRAPH3_INSTALL_CMAKEDIR}")
