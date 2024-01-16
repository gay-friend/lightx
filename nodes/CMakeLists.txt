cmake_minimum_required(VERSION 3.5) # cmake最低版本要求
set(PROJECT_NAME node) #设置工程环境变量
project(${PROJECT_NAME} VERSION 0.1) # 设置工程名称

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find qt
list(APPEND CMAKE_PREFIX_PATH ~/Qt/6.5.3/gcc_64)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(OpenCV REQUIRED)

add_library(${PROJECT_NAME} SHARED node.cpp node_widget.cpp port.cpp  ${ROOT}/include/nodes/node.h ${ROOT}/include/nodes/node_widget.h)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS} ${OpenCV_LIBS})

add_subdirectory(camera)
add_subdirectory(image)