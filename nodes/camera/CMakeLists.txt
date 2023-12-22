set(PROJECT_NAME camera) #设置工程环境变量
project(${PROJECT_NAME} VERSION 0.1) # 设置工程名称

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/lib)

add_library(${PROJECT_NAME} SHARED camera_node.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})