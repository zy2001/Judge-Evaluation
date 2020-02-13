cmake_minimum_required(VERSION 3.15)
project(Judge)

set(CMAKE_CXX_STANDARD 14)
#set(MYSQL_INCLUDE_DIR C:\\CodeStudio\\mysql-8.0.17-winx64\\include)
#set(MYSQL_LIB_DIR C:\\CodeStudio\\mysql-8.0.17-winx64\\lib)
#include_directories(${MYSQL_INCLUDE_DIR})
include_directories(include/librabbitmq)
include_directories(include/rapidjson)

#link_directories(${MYSQL_LIB_DIR})
#link_libraries(libmysql)

link_directories(lib/librabbitmq)
#link_libraries(rabbitmq.4)

add_executable(Judge main.cpp src/header/compile.h src/source/compile.cpp src/config.h src/header/manager.h src/source/manager.cpp src/header/judgecore.h src/source/judgecore.cpp src/header/utils.h src/source/utils.cpp src/header/rabbitmq.h src/source/rabbitmq.cpp src/header/domain.h)


target_link_libraries(Judge -lrabbitmq.4)
#target_link_libraries(Judge -llibmysql)
target_link_libraries(Judge -lpsapi)