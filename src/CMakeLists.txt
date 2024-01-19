set(PROJECT_NAME lightx) #设置工程环境变量
project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX) # 设置工程名称

# 发现一个目录下所有的源代码文件并将列表存储在一个变量中
file(GLOB_RECURSE SRCS ${ROOT}/src/*.*pp)
foreach(file ${SRCS})
    message(${file})
endforeach()

add_executable(${PROJECT_NAME} ${SRCS} ${INCLUDES} ${THIRD_PARTY_INCLUDES})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE node)
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
