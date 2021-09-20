cmake_minimum_required(VERSION 3.11)
Project(Client)

include(FetchContent)

FetchContent_Declare(
        cppzmq
        GIT_REPOSITORY https://github.com/zeromq/cppzmq.git
        GIT_SHALLOW	ON
        GIT_TAG v4.6.0
)

FetchContent_GetProperties(cppzmq)
if(NOT cppzmq_POPULATED)
    message(STATUS "Populating zmq-cpp...")
    FetchContent_Populate(cppzmq)
    # Add here options for cppzmq building
    set(CPPZMQ_BUILD_TESTS OFF CACHE BOOL "Whether or not to build the tests")
    add_subdirectory(${cppzmq_SOURCE_DIR} ${cppzmq_BINARY_DIR})
    message(STATUS "Done.")
endif()

include_directories(BEFORE SYSTEM "${cppzmq_SOURCE_DIR}")
target_link_libraries(${ZMQ} cppzmq)

include(FetchContent)
FetchContent_Declare(
    flatbuffers
    GIT_REPOSITORY "https://github.com/google/flatbuffers"
    GIT_TAG        v2.0.0
)
set(FLATBUFFERS_BUILD_FLATC ON CACHE INTERNAL "Enable the build of the flatbuffers compiler")
set(FLATBUFFERS_BUILD_TESTS OFF CACHE INTERNAL "Enable tests from flatbuffers lib.")
set(FLATBUFFERS_BUILD_FLATHASH OFF CACHE INTERNAL "Enable the build of flathash")
set(FLATBUFFERS_ENABLE_PCH OFF CACHE INTERNAL
       "Enable precompile headers support for 'flatbuffers' and 'flatc'. \"
        Only work if CMake support 'target_precompile_headers'. \"
        Only work if CMake supports 'target_precompile_headers'. \"
        This can speed up compilation time."
       )

FetchContent_MakeAvailable(flatbuffers)

include_directories(BEFORE SYSTEM "${flatbuffers_SOURCE_DIR}/include")

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/channel_generated.h
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/channel.fbs
    COMMAND flatc --cpp ${CMAKE_CURRENT_SOURCE_DIR}/channel.fbs
)

add_custom_target(channelbuffer DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/channel_generated.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(Client client.cpp)
target_link_libraries(Client cppzmq)
add_dependencies(Client channelbuffer)
