CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(cc_server)

SET(CMAKE_CXX_FLAGS "-g -Wall -DDEBUG")

INCLUDE_DIRECTORIES(
	include
	../../vas/include/
)

AUX_SOURCE_DIRECTORY(src/ SRC_LIST)
LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../vas/build)

ADD_EXECUTABLE(cc_server.exe ${SRC_LIST})
TARGET_LINK_LIBRARIES(cc_server.exe vas jsoncpp pthread ssl crypto)
