#include "ServerCommunication.h"

using boost::asio::ip::tcp;


size_t ServerCommunication::read_buffer(boost::asio::ip::tcp::socket& sock, std::vector<unsigned char>& buffer, size_t buffer_size)
{
    try {
        if (buffer.size() != buffer_size) {
            throw std::invalid_argument("Error while reading from server");
        }
        return boost::asio::read(sock, boost::asio::buffer(buffer, buffer_size));
    }
    catch (boost::system::system_error const& e) {
        std::cout << "Error while reading from server" << std::endl;
        throw;
    }
}

size_t ServerCommunication::write_buffer(boost::asio::ip::tcp::socket& sock, std::vector<unsigned char>& buffer)
{
    try { 
        return boost::asio::write(sock, boost::asio::buffer(buffer));
    }
    catch (boost::system::system_error const& e) {
        std::cout << "Error while writing to server" << std::endl;
        throw;
    }
}
