cmake_minimum_required(VERSION 3.5) # cmake最低版本要求

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(PROJECT_NAME lightx) #设置工程环境变量
project(${PROJECT_NAME} VERSION 0.1) # 设置工程名称

set(ROOT ${PROJECT_SOURCE_DIR})


IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	MESSAGE(STATUS "current platform: Linux ")
    file(GLOB_RECURSE LIBS ${PROJECT_SOURCE_DIR}/../light-thirdparty/libs/linux/*so)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	MESSAGE(STATUS "current platform: Windows")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	MESSAGE(STATUS "current platform: FreeBSD")
ELSE ()
    MESSAGE(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Linux")

foreach(file ${LIBS})
    message(${file})
endforeach()

include_directories(${PROJECT_SOURCE_DIR}/include) # 添加头文件
include_directories(${PROJECT_SOURCE_DIR}/../light-thirdparty/include)

add_subdirectory(${PROJECT_SOURCE_DIR}/nodes node)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)

add_subdirectory(${PROJECT_SOURCE_DIR}/examples)
