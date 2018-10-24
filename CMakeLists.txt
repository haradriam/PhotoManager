CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

######## Project settings ########
PROJECT(PhotoManager)

SET(VERSION_MAJOR 1)
SET(VERSION_MINOR 0)
SET(VERSION ${VERSION_MAJOR}.${VERSION_MINOR})

SET(CMAKE_CXX_FLAGS "${CMAKE_CPP_FLAGS} -std=gnu++11 -Wall -Wextra -Werror")


######## Include dirs ########
SET(INC_DIRECTORIES ${INC_DIRECTORIES} include)


######## Libraries ########
SET(LINK_LIBRARIES ${LIBS_SYSTEM} exif)


######## Build and include settings ########
INCLUDE_DIRECTORIES(${INC_DIRECTORIES})


######## Targets ########
FILE(GLOB SOURCES "src/*.cpp")

ADD_EXECUTABLE(PhotoManager ${SOURCES})
TARGET_LINK_LIBRARIES(PhotoManager ${LINK_LIBRARIES})
