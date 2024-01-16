# 发现一个目录下所有的源代码文件并将列表存储在一个变量中
file(GLOB_RECURSE SRCS ${ROOT}/src/*.cpp
                       ${ROOT}/src/*.hpp
                       ${ROOT}/include/entity/*.hpp
                    #    ${ROOT}/include/qwidgets/*.hpp
                       ${ROOT}/include/widgets/*.h
                       ${ROOT}/include/model/*.h
                       )

foreach(file ${SRCS})
    message(${file})
endforeach()


# find qt
list(APPEND CMAKE_PREFIX_PATH ~/Qt/6.5.3/gcc_64)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE node)
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS} ${OpenCV_LIBS})
