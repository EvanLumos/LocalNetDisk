
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

ADD_SUBDIRECTORY( ${PROJECT_SOURCE_DIR}/src/base )
ADD_SUBDIRECTORY( ${PROJECT_SOURCE_DIR}/src/net )

include_directories(${PROJECT_SOURCE_DIR}/src/base)
include_directories(${PROJECT_SOURCE_DIR}/src/net)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(LND_client LND_client.cpp)
target_link_libraries(LND_client muduo_base muduo_net)

add_executable(LND_server LND_server.cpp)
target_link_libraries(LND_server muduo_base muduo_net)

