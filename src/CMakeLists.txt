# 发现一个目录下所有的源代码文件并将列表存储在一个变量中
file(GLOB_RECURSE SRCS ${ROOT}/src/*.*pp)
foreach(file ${SRCS})
    message(${file})
endforeach()

add_executable(${PROJECT_NAME} ${SRCS} ${INCLUDES} ${THIRD_PARTY_INCLUDES})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE node ${LIBS})
