#
#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

ADD_SUBDIRECTORY( ${PROJECT_SOURCE_DIR}/src/base )
ADD_SUBDIRECTORY( ${PROJECT_SOURCE_DIR}/src/net )
ADD_SUBDIRECTORY( ${PROJECT_SOURCE_DIR}/src/client )
ADD_SUBDIRECTORY( ${PROJECT_SOURCE_DIR}/src/server )

#include_directories(${PROJECT_SOURCE_DIR}/src/base)
#include_directories(${PROJECT_SOURCE_DIR}/src/net)
#
#find_package(Boost REQUIRED)
#include_directories(${Boost_INCLUDE_DIRS})





