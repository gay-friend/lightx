cmake_minimum_required(VERSION 3.5) # cmake最低版本要求
set(PROJECT_NAME lightx) #设置工程环境变量
project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX) # 设置工程名称

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(ROOT ${PROJECT_SOURCE_DIR})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/../lightx-thirdparty)
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	MESSAGE(STATUS "current platform: Linux ")
    set(BUILD_DIR ${ROOT}/../build/linux)
    file(GLOB_RECURSE LIBS ${THIRD_PARTY_DIR}/libs/linux/*so*)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	MESSAGE(STATUS "current platform: Windows")
    set(BUILD_DIR ${ROOT}/../build/win)
    file(GLOB_RECURSE LIBS ${THIRD_PARTY_DIR}/libs/win/*lib)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	MESSAGE(STATUS "current platform: FreeBSD")
ELSE ()
    MESSAGE(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Linux")

# 设置动态库的输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR})
# 设置静态库的输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR})
set(EXECUTABLE_OUTPUT_PATH ${BUILD_DIR})

foreach(file ${LIBS})
    message(${file})
endforeach()

include_directories(${PROJECT_SOURCE_DIR}/include) # 添加头文件
include_directories(LIBS ${THIRD_PARTY_DIR}/include)

file(GLOB_RECURSE THIRD_PARTY_INCLUDES ${THIRD_PARTY_DIR}/include/*.h*)
# foreach(file ${THIRD_PARTY_INCLUDES})
#     message(${file})
# endforeach()

file(GLOB_RECURSE INCLUDES ${ROOT}/include/*.h*)
# foreach(file ${INCLUDES})
#     message(${file})
# endforeach()

add_subdirectory(node)
add_subdirectory(src)
# add_subdirectory(${PROJECT_SOURCE_DIR}/examples)
