#include "message.hpp"
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;


void async_read(tcp::socket &socket) {
    auto buffer = std::make_shared<boost::asio::streambuf>();

    auto handler = [&socket, buffer](boost::system::error_code ec, std::size_t length) {
        if (!ec) {
            std::istream is(buffer.get()); // istream object "is" is created 
            std::string received;
            std::getline(is, received); // reads characters from stram "is" until \n 
            std::cout << "Server: " << received << std::endl;
            async_read(socket); // recursive call for continuous reading
        }
    };

    boost::asio::async_read_until(socket, *buffer, "\n", handler);
}




int main(int argc, char* argv[]){
    if(argc < 2){
        std::cerr << "Provide port too as second argument" << std::endl;
        return 1;
    }
    
    boost::asio::io_context io_context;
    tcp::socket socket(io_context);
    tcp::resolver resolver(io_context);

    boost::asio::connect(socket, resolver.resolve("127.0.0.1", argv[1]));
    // 127.0.0.1 -->always points back to the same machine you’re
    
    async_read(socket);

    std::thread t([&io_context, &socket]() {
        while (true) {
            std::string data;
            std::cout << "Enter message: ";
            std::getline(std::cin, data);
            data += "\n";

            auto writer = [&, data]() {
                boost::asio::write(socket, boost::asio::buffer(data));
            };

            // schedule writer on the io_context thread
            boost::asio::post(io_context, writer);
        }
    });



    io_context.run();
    t.join();

    return 0;
}