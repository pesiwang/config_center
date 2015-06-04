CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(cc_core)

SET(CMAKE_CXX_FLAGS "-g -Wall -DDEBUG")

INCLUDE_DIRECTORIES(
	include
	../../lib/include
)

LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/build)

ADD_EXECUTABLE(write_to_sharedmemory.exe src/WriteToSharedMemory.cpp)
ADD_EXECUTABLE(write_to_ccserver.exe src/WriteToCCServer.cpp)
TARGET_LINK_LIBRARIES(write_to_sharedmemory.exe cc jsoncpp)
TARGET_LINK_LIBRARIES(write_to_ccserver.exe cc jsoncpp z ssl crypto)
