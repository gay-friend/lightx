set(PROJECT_NAME node${BUILD_FLAG}) #设置工程环境变量
project(${PROJECT_NAME} VERSION 0.1) # 设置工程名称

# 发现一个目录下所有的源代码文件并将列表存储在一个变量中
file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/../include/node/*.h)
foreach(file ${SRCS})
    message(${file})
endforeach()

add_library(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${THRRD_PARTY_LIB})