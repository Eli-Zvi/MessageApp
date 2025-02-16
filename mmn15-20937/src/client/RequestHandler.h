#pragma once
/**
* @author Ilay Zvi
* @file RequestHandler.h
* contents:
* namespace - RequestHandler : handles all user requests to the server, and the responses given by the server
*
*/
#include <vector>
#include "Definitions.h"
#include "Client.h"
#include "Encryption.h"
#include <boost/asio.hpp>
#include <boost/endian/conversion.hpp>
#include "Message.h"
#include <string>
#include "UserIO.h"
#include "ServerCommunication.h"
#include "exceptions.h"
#include <stdexcept>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <sstream>

using boost::asio::ip::tcp;

namespace RequestHandler {
	/*
	* establishes connection and runs the main loop of the client.
	* parses requests from client and diverts to the appropriate handler.
	* catches exceptions and displays messages accordingly
	*/
	void call_handler();
	
	/*
	* builds a response header for a request
	*/
	static void build_header(std::vector<unsigned char>& buffer, const types::uuid client_id, RequestCode code, size_t payload_size);

	/*
	* handles a user registration request and the response from the server
	*/
	static Client registration_handler(types::username name, tcp::socket& sock);

	/*
	* handles a user client list request and the response from the server
	*/
	static void client_list_handler(Client& c, tcp::socket& sock);

	/*
	* handles a user public key request and the response from the server
	*/
	static void pub_key_handler(Client& c, tcp::socket& sock);

	/*
	* handles a user message send request and the response from the server from 3 different types(150/151/152) 
	* txt msg,symmetric key request, symmetric key response and encrypts messages 2 and 3
	*/
	static void message_send_handler(Client& c, tcp::socket& sock, MessageType msg_type);

	/*
	* handles user's awaiting message request and the response from the server
	*/
	static void awaiting_message_handler(Client& c, tcp::socket& sock);

	/*
	* parses the contents of the message sent by another user
	*/
	std::string parse_message_content(std::string content, MessageType type, Contact& sender, Client& c);

	/*
	* reads and parses response header
	*/
	static std::tuple<ResponseCode, uint32_t> parse_response_header(tcp::socket& sock);

	/*
	* reads and returns response payload
	*/
	static std::vector<unsigned char> parse_response_payload(tcp::socket& sock, size_t buffer_size);
}