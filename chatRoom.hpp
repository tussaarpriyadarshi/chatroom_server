#include <iostream>
#include "message.hpp"    

#ifndef CHATROOM_HPP
#define CHATROOM_HPP
#include <set>
#include <memory>
#include <sys/socket.h>
#include<unistd.h>
#include<boost/asio.hpp>
#include <deque>

#define CHATROOM_HPP
using boost::asio::ip::tcp;

 class Participant{
    public:
    virtual void deliver(Message &message)=0;
    virtual void write(Message &message)=0;
    virtual ~Participant()=default;
 };
 typedef std::shared_ptr<Participant>ParticipantPtr;



 class Room{
    public:
     void join(ParticipantPtr Participant);
     void leave(ParticipantPtr Participant);
     void deliver(ParticipantPtr Participant,Message &message);
     public:
     std::deque<Message>messageQueue;
     enum {MaxParticipants=100};
     std::set<ParticipantPtr>participants;
 };


class Session :public Participant,public std::enable_shared_from_this<Session>{
    public:
        Session(tcp::socket s,Room &room);
        void start();
        void deliver(Message &message);
        void write(Message &message);
        void async_read();
        void async_write(std::string messageBody,size_t mesageLength);
        private:
        tcp::socket clientSocket;
        boost::asio::streambuf buffer;
        Room &room;
        std::deque<Message>messageQueue;

};






#endif 