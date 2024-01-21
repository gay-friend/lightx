cmake_minimum_required(VERSION 3.5) # cmake最低版本要求

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
	set(LIB_SUBDIR "Debug")
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
	set(LIB_SUBDIR "Release")
ELSE()
	set(LIB_SUBDIR "Debug")
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)

IF(CMAKE_SYSTEM_NAME MATCHES Linux)
    set(SYS_SUFFIX linux)
    set(LIB_SUFFIX *.so*)
    set(BIN_SUFFIX *.so*)
ELSE()
    set(SYS_SUFFIX win)
    set(LIB_SUFFIX *.lib)
    set(BIN_SUFFIX *.dll)
ENDIF(CMAKE_SYSTEM_NAME MATCHES Linux)



set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/../lightx-thirdparty)
set(OUTPUT_DIR ${PROJECT_SOURCE_DIR}/../build/${SYS_SUFFIX}/${LIB_SUBDIR})
file(GLOB_RECURSE THRRD_PARTY_LIB ${THIRD_PARTY_DIR}/lib/${LIB_SUFFIX})
file(GLOB_RECURSE THRRD_PARTY_BIN ${THIRD_PARTY_DIR}/bin/${BIN_SUFFIX})
message(Output: ${OUTPUT_DIR})
message(THIRD_PARTY_DIR: ${THIRD_PARTY_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../build/${SYS_SUFFIX})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../build/${SYS_SUFFIX}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../build/${SYS_SUFFIX}/lib)

foreach(file ${THRRD_PARTY_LIB})
    message(${file})
endforeach()
foreach(file ${THRRD_PARTY_BIN})
    message(${file})
endforeach()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

include_directories(${PROJECT_SOURCE_DIR}/include) # 添加头文件
include_directories(${THIRD_PARTY_DIR}/include)

file(GLOB_RECURSE THIRD_PARTY_INCLUDES ${THIRD_PARTY_DIR}/include/*.h*)
file(GLOB_RECURSE INCLUDES ${PROJECT_SOURCE_DIR}/include/*.h*)

add_subdirectory(node)
add_subdirectory(src)
# add_subdirectory(${PROJECT_SOURCE_DIR}/examples)

file(COPY ${THRRD_PARTY_BIN} DESTINATION ${OUTPUT_DIR})
