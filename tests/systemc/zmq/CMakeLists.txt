cmake_minimum_required(VERSION 3.12)
project(zmq CXX)

find_package(cppzmq REQUIRED)
if (${cppzmq_FOUND})

    message(STATUS "Using ZMQ for IPC")
    add_definitions(-DUSE_ZMQ)

    macro(add_executable _name)
        # invoke built-in add_executable
        _add_executable(${ARGV})
        if (TARGET ${_name})
            target_link_libraries(${_name} cppzmq)
        endif ()
    endmacro()
endif ()

add_executable(ram.o ${CMAKE_BINARY_DIR}/blackboxes/ram_driver.cpp)
target_link_libraries(ram.o SystemC::systemc)

add_definitions(-DELEM_LIB="systemc${CMAKE_PROJECT_NAME}")
add_executable(libsystemc${CMAKE_PROJECT_NAME}.so ../tests/unit.cpp ${CMAKE_BINARY_DIR}/blackboxes/ram_comp.cpp)
_sst_compile_link(libsystemc${CMAKE_PROJECT_NAME}.so)
