set(PROJECT_NAME examples)
project(${PROJECT_NAME} VERSION 0.1)

# 发现一个目录下所有的源代码文件并将列表存储在一个变量中
file(GLOB_RECURSE SRCS *.cpp
                       ${PROJECT_SOURCE_DIR}/../include/qwidgets/*.hpp
                       )

foreach(file ${SRCS})
    message(${file})
endforeach()
message(====================================${PROJECT_NAME})

# find qt
list(APPEND CMAKE_PREFIX_PATH ~/Qt/6.5.3/gcc_64)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_library(test SHARED test.cpp)
add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
