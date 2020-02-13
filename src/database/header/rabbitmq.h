//
// Created by zy on 2020/2/12.
//

#ifndef JUDGE_RABBITMQ_H
#define JUDGE_RABBITMQ_H

#include <amqp.h>
#include <queue>

class RabbitMQ {
private:
    static amqp_connection_state_t conn;
//    static std::queue<amqp_channel_t> q;
public:

    static bool connect();

    static char *read_message();

    static bool sendMessage(const std::string str);

    static bool close();
};



//bool rabbitmq_send

#endif //JUDGE_RABBITMQ_H
