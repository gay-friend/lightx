set(PROJECT_NAME split_image) #设置工程环境变量
project(${PROJECT_NAME} VERSION 0.1) # 设置工程名称

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/lib)

add_library(${PROJECT_NAME} SHARED split_image_node.cpp ${ROOT}/include/nodes/split_image_node.h)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})

add_library(image_convert SHARED image_convert.cpp ${ROOT}/include/nodes/image_convert.h)
target_link_libraries(image_convert PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(image_convert PRIVATE ${LIBS})

add_library(image_threshold SHARED threshold_node.cpp ${ROOT}/include/nodes/threshold_node.h)
target_link_libraries(image_threshold PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(image_threshold PRIVATE ${LIBS})