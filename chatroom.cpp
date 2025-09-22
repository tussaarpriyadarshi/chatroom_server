#include "chatRoom.hpp"

// ===== Room methods =====
void Room::join(ParticipantPtr Participant) {
    this->participants.insert(Participant);
}

void Room::leave(ParticipantPtr Participant) {
    this->participants.erase(Participant);
}

// Except the sender, send the message to all
void Room::deliver(ParticipantPtr Participant, Message &message) {
    messageQueue.push_back(message);
    while (!messageQueue.empty()) {
        Message msg = messageQueue.front();
        messageQueue.pop_front();
        for (ParticipantPtr _participant : participants) {
            if (Participant != _participant) {
                _participant->write(msg);
            }
        }
    }
}

// ===== Session methods =====

// Constructor definition (previously missing)
Session::Session(tcp::socket s, Room &room)
    : clientSocket(std::move(s)), room(room) {
}

void Session::async_read() {
    auto self(shared_from_this());
    boost::asio::async_read_until(clientSocket, buffer, "\n",
        [this, self](boost::system::error_code ec, std::size_t bytes_transferred) {
            if (!ec) {
                std::string data(boost::asio::buffers_begin(buffer.data()),
                                 boost::asio::buffers_begin(buffer.data()) + bytes_transferred);
                buffer.consume(bytes_transferred);
                std::cout << "received: " << data << std::endl;
                Message message(data);
                deliver(message);
                async_read();
            } else {
                room.leave(shared_from_this());
                if (ec == boost::asio::error::eof) {
                    std::cout << "connection closed" << std::endl;
                } else {
                    std::cout << "read error: " << ec.message() << std::endl;
                }
            }
        }
    );
}

void Session::async_write(std::string messageBody, size_t messageLength) {
    auto write_handler = [&](boost::system::error_code ec, std::size_t) {
        if (!ec) {
            std::cout << "data is written to the socket" << std::endl;
        } else {
            std::cerr << "write error: " << ec.message() << std::endl;
        }
    };
    boost::asio::async_write(clientSocket, boost::asio::buffer(messageBody, messageLength), write_handler);
}

// Not directly sending message to client, putting in the queue then sending
void Session::write(Message &message) {
    messageQueue.push_back(message);
    while (!messageQueue.empty()) {  // FIX: condition corrected
        Message msg = messageQueue.front();
        messageQueue.pop_front();
        bool header_decode = msg.decodeHeader();
        if (header_decode) {
            std::string body = msg.getBody();
            async_write(body, msg.getBodyLength());
        } else {
            std::cout << "Message length exceeds";
        }
    }
}

void Session::deliver(Message &message) {
    room.deliver(shared_from_this(), message);
}

void Session::start() {
    room.join(shared_from_this());
    async_read();
}

// ===== Accept connection function =====
void accept_connection(boost::asio::io_context &io, char *port,
                       tcp::acceptor &acceptor, Room &room, const tcp::endpoint &endpoint) {
    acceptor.async_accept(
        [&](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                auto session = std::make_shared<Session>(std::move(socket), room);
                session->start();
            }
            accept_connection(io, port, acceptor, room, endpoint);
        }
    );
}

// ===== main =====
int main(int argc, char *argv[]) {
    try {
        if (argc < 2) {
            std::cerr << "Usage: server <port>\n";
            return 1;
        }
        Room room;
        boost::asio::io_context io_context;
        tcp::endpoint endpoint(tcp::v4(), atoi(argv[1]));
        tcp::acceptor acceptor(io_context, endpoint);
        accept_connection(io_context, argv[1], acceptor, room, endpoint);
        io_context.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
