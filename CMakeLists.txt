cmake_minimum_required(VERSION 3.15)
project(Judge)

set(CMAKE_CXX_STANDARD 14)
set(MYSQL_INCLUDE_DIR C:\\CodeStudio\\mysql-8.0.17-winx64\\include)
set(MYSQL_LIB_DIR C:\\CodeStudio\\mysql-8.0.17-winx64\\lib)
include_directories(${MYSQL_INCLUDE_DIR})
include_directories(include/librabbitmq)
include_directories(include/rapidjson)

link_directories(${MYSQL_LIB_DIR})
#link_libraries(libmysql)

link_directories(lib/librabbitmq)
#link_libraries(rabbitmq.4)

add_executable(Judge main.cpp src/compile/header/compile.h src/compile/source/compile.cpp src/config.h src/manager/header/manager.h src/manager/source/manager.cpp src/judge/header/judgecore.h src/judge/source/judgecore.cpp src/utils/header/utils.h src/utils/source/utils.cpp src/database/header/rabbitmq.h src/database/source/rabbitmq.cpp)


target_link_libraries(Judge -lrabbitmq.4)
target_link_libraries(Judge -llibmysql)
target_link_libraries(Judge -lpsapi)