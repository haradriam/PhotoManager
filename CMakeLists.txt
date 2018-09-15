CMAKE_MINIMUM_REQUIRED(VERSION 3.3.2)

######## Project settings ########
PROJECT(PhotoManager)

SET(VERSION_MAJOR 1)
SET(VERSION_MINOR 0)
SET(VERSION ${VERSION_MAJOR}.${VERSION_MINOR})

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CPP_FLAGS "${CMAKE_CPP_FLAGS} -Wall -Wextra -Werror")



######## Include dirs ########
SET(INC_DIRECTORIES ${INC_DIRECTORIES} include)


######## Linker dirs ########
SET(LINK_DIRECTORIES ${LINK_DIRECTORIES} lib)


######## Libraries ########
SET(LINK_LIBRARIES ${LIBS_SYSTEM})


######## Build and include settings ########
INCLUDE_DIRECTORIES(${INC_DIRECTORIES})
LINK_DIRECTORIES(${LINK_DIRECTORIES})


######## Targets ########
FILE(GLOB SOURCES "src/*.cpp")

ADD_EXECUTABLE(PhotoManager ${SOURCES})
