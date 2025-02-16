#pragma once
/**
* @author Ilay Zvi
* @file ServerCommunication.h
* contents:
* namespace : ServerCommunication - defines methods for sending data to the server and receiving data from the server 
*
*/
#include <boost/asio.hpp>
#include <vector>
#include <iostream>
#include <stdexcept>

namespace ServerCommunication {
	/*
	* reads buffer of size buffer_size to buffer from socket sock
	* @returns size of buffer
	*/
	size_t read_buffer(boost::asio::ip::tcp::socket& sock, std::vector<unsigned char>& buffer, size_t buffer_size);

	/*
	* writes data stored in buffer to sock
	* @returns size of buffer
	*/
	size_t write_buffer(boost::asio::ip::tcp::socket& sock, std::vector<unsigned char>& buffer);
}