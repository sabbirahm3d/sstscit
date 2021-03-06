cmake_minimum_required(VERSION 3.12)
project(SIT CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Define the library target
add_library(SIT INTERFACE)
target_include_directories(SIT INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sit>
    $<INSTALL_INTERFACE:sit>
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/SITConfigVersion.cmake"
    VERSION $ENV{SIT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(
    TARGETS SIT
    EXPORT SITTargets
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    RUNTIME DESTINATION bin COMPONENT Runtime
    PUBLIC_HEADER DESTINATION include COMPONENT Development
    BUNDLE DESTINATION bin COMPONENT Runtime
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/SITConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/SITConfig.cmake"
    INSTALL_DESTINATION lib/cmake/SIT
)

install(
    EXPORT SITTargets
    DESTINATION lib/cmake/SIT
)
install(
    FILES "${PROJECT_BINARY_DIR}/SITConfigVersion.cmake" "${PROJECT_BINARY_DIR}/SITConfig.cmake"
    DESTINATION lib/cmake/SIT
)

execute_process(
    COMMAND which sst-config
    OUTPUT_VARIABLE SST_CONFIG
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND ${SST_CONFIG} --prefix
    OUTPUT_VARIABLE _PREFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND ${SST_CONFIG} --includedir
    OUTPUT_VARIABLE INCLUDEDIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
get_filename_component(INCLUDEDIR "${INCLUDEDIR}" NAME)
set(INCLUDEDIR "${_PREFIX}/${INCLUDEDIR}")

install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/sit/
    DESTINATION ${INCLUDEDIR}/sst/sit
)

find_package(Python3 REQUIRED)
if (Python3_FOUND)
    set(VENV_DIR ${PROJECT_SOURCE_DIR}/.venv)
    execute_process(
        COMMAND ${Python3_EXECUTABLE} -m venv ${VENV_DIR}
    )
    execute_process(
        COMMAND ${VENV_DIR}/bin/pip freeze
        OUTPUT_VARIABLE PIP_OUTPUT
    )
    string(FIND "${PIP_OUTPUT}" "boilerplate" MOD_FOUND)
    if ("${MOD_FOUND}" STREQUAL "-1")
        execute_process(
            COMMAND ${VENV_DIR}/bin/pip install .
            WORKING_DIRECTORY ${INCLUDEDIR}/sst/sit
        )
    endif()
endif()

include(tests/CMakeLists.txt)
