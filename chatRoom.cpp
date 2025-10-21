#include "chatRoom.hpp"

using boost::asio::ip::tcp;
using boost::asio::ip::address_v4;

// ================== Accept Connection ==================
void accept_connection(boost::asio::io_context &io,char *port,tcp::acceptor &acceptor,Room &room,const tcp::endpoint& endpoint)
{
    auto handle_accept = [&](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            auto session = std::make_shared<Session>(std::move(socket), room); // Create a new session for this client
            session->start();
        }
        accept_connection(io, port, acceptor, room, endpoint);
    };

    acceptor.async_accept(handle_accept);
}

// ================== Session Class ==================
Session::Session(tcp::socket s, Room& r)
    : clientSocket(std::move(s)), room(r) {}

void Session::start() {
    room.join(shared_from_this()); // Join the room
    async_read();
}

// ----- Async Read -----
void Session::async_read() {
    auto self(shared_from_this());

    auto handle_read = [this, self](boost::system::error_code ec, std::size_t bytes_transferred) {
        if (!ec) {
            std::string data(boost::asio::buffers_begin(buffer.data()), boost::asio::buffers_begin(buffer.data()) + bytes_transferred);
            buffer.consume(bytes_transferred);
            std::cout << "Received: " << data << std::endl;

            Message message(data);
            deliver(message);
            async_read(); 
        } 
        else {
            room.leave(shared_from_this());
            if (ec == boost::asio::error::eof) {
                std::cout << "Connection closed by peer" << std::endl;
            } else {
                std::cout << "Read error: " << ec.message() << std::endl;
            }
        }
    };

    boost::asio::async_read_until(clientSocket, buffer, "\n", handle_read);
}

// ----- Deliver to Room -----
void Session::deliver(Message& incomingMessage){
    room.deliver(shared_from_this(), incomingMessage);
}

// ----- Write to Client -----
void Session::write(Message &message){
    messageQueue.push_back(message);
    while(!messageQueue.empty()){
        Message message = messageQueue.front();
        messageQueue.pop_front();

        bool header_decode = message.decodeHeader();
        if(header_decode){
            std::string body = message.getBody();
            async_write(body, message.getBodyLength());
        } else {
            std::cout << "Message length exceeds the max length" << std::endl;
        }
    }
}

// ----- Async Write -----
void Session::async_write(std::string messageBody, size_t messageLength){
    auto write_handler = [&](boost::system::error_code ec, std::size_t bytes_transferred){
        if(!ec){
            std::cout << "Data written to the socket" << std::endl;
        } else {
            std::cerr << "Write error: " << ec.message() << std::endl;
        }
    };

    boost::asio::async_write(clientSocket,boost::asio::buffer(messageBody, messageLength),write_handler);
}

// ================== Room Class ==================
void Room::join(ParticipantPointer participant){
    participants.insert(participant);
}

void Room::leave(ParticipantPointer participant){
    participants.erase(participant);
}


void Room::deliver(ParticipantPointer participant, Message &message){
        for (ParticipantPointer _participant : participants) {
            if (participant != _participant) {
                _participant->write(message);
            }
        }
}

// ================== Main ==================
int main(int argc, char *argv[]) {
    try {
        if(argc < 2) {
            std::cerr << "Usage: server <port>\n";
            return 1;
        }

        Room room;
        boost::asio::io_context io_context;

        tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[1]));
        tcp::acceptor acceptor(io_context, endpoint);

        // Start accepting clients
        accept_connection(io_context, argv[1], acceptor, room, endpoint);

        // Run the IO event loop
        io_context.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
