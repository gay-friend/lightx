cmake_minimum_required(VERSION 3.5) # cmake最低版本要求

# set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(PROJECT_NAME lightx) #设置工程环境变量
project(${PROJECT_NAME} VERSION 0.1) # 设置工程名称

set(ROOT ${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE LIBS ${PROJECT_SOURCE_DIR}/lib/*so)
foreach(file ${LIBS})
    message(${file})
endforeach()

include_directories(${PROJECT_SOURCE_DIR}/include) # 添加头文件

add_subdirectory(${PROJECT_SOURCE_DIR}/nodes node)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)

add_subdirectory(${PROJECT_SOURCE_DIR}/examples)
