//
// Created by zy on 2020/2/12.
//

#include <amqp.h>
#include <string>
#include <amqp_tcp_socket.h>
#include <iostream>
#include <cstring>
#include "../header/rabbitmq.h"


amqp_connection_state_t  RabbitMQ::conn;

bool RabbitMQ::connect() {
    std::string hostName = "121.36.13.125";
    int port = 5672;
    amqp_socket_t *socket = nullptr;
    RabbitMQ::conn = amqp_new_connection();
    socket = amqp_tcp_socket_new(RabbitMQ::conn);
    if (!socket) {
        std::cout << "create socket failed!";
        return false;
    }
    if (amqp_socket_open(socket, hostName.c_str(), port)) {
        std::cout << "opening TCP socket failed" << std::endl;
        return false;
    }
    //登录
    if (1 != amqp_login(RabbitMQ::conn, "judge", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, "judge", "151814").reply_type) {
        std::cout << "login failed" << std::endl;
        return false;
    }
    amqp_channel_open(RabbitMQ::conn, 1);
    return true;
}

char *RabbitMQ::read_message() {
    char *result = nullptr;
    amqp_rpc_reply_t ret = amqp_basic_get(RabbitMQ::conn, 1, amqp_cstring_bytes("judgeItem"), 1);
    if (ret.reply_type == AMQP_RESPONSE_NORMAL && ret.reply.id == 3932231) {
        amqp_message_t *msg = new amqp_message_t;
        amqp_read_message(RabbitMQ::conn, 1, msg, 0);
        result = new char[msg->body.len + 1];
        result[msg->body.len] = 0;
        strncpy(result, (char *) msg->body.bytes, msg->body.len);
        amqp_destroy_message(msg);
        delete msg;
    }
    return result;
}

bool RabbitMQ::close() {
    amqp_channel_close(RabbitMQ::conn, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(RabbitMQ::conn, AMQP_REPLY_SUCCESS);
    return true;
}

bool RabbitMQ::sendMessage(std::string str) {
    std::cout<<str<<std::endl;
    char *message = new char[str.size()];
    str.copy(message, str.size(), 0);
    amqp_bytes_t message_bytes;
    message_bytes.len = str.size();
    message_bytes.bytes = message;
    amqp_basic_publish(conn, 1, amqp_cstring_bytes(""), amqp_cstring_bytes("judgeStatus"), 0, 0, nullptr,
                       message_bytes);
    delete[] message;
    return true;
}
