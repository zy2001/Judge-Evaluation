//
// Created by zy on 2020/1/31.
//

#include "src/header/manager.h"
#include "src/header/rabbitmq.h"

using namespace std;

int main() {
    // 连接消息队列
    RabbitMQ::connect();
    // 初始化管理器
    Manager manager;
    // 运行管理器
    manager.run();
    // 关闭消息队列
    RabbitMQ::close();
    return 0;
}

