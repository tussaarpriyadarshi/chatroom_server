#include "message.hpp"
#include <iostream>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
void async_read(tcp::socket &socket)
{
    auto buffer = std::make_shared<boost::asio::streambuf>();
    boost::asio::async_read_until(socket, *buffer, "\n", [&socket, buffer](boost::system::error_code ec, std::size_t length)
                                  {
        if(!ec){
            std::istream is(buffer.get());
            std::string recieved;
            std::getline(is,recieved);
            std::cout<<"server"<<recieved<<std::endl;
            async_read(socket);
        } });
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "provide port too as second argument" << std::endl;
        return 1;
    }
    boost::asio::io_context io_context;
    tcp::socket socket(io_context);
    tcp::resolver resolver(io_context);

    boost::asio::connect(socket, resolver.resolve("127.0.0.1", argv[1]));
    async_read(socket);
    std::thread t([&io_context,&socket](){
        while (true)
        {
            std::string data;
            std::cout << "enter message:";
            std::getline(std::cin, data);
            data += "\n";
            boost::asio::post(io_context, [&, data]()
                              { boost::asio::write(socket, boost::asio::buffer(data));
                             
                            
                            });
        }

});
io_context.run();
t.join();
return 0;
}
