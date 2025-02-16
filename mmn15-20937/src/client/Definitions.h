#pragma once
/**
* @author Ilay Zvi
* @file Definitions.h
* contents:
* namespace - constants : defines the constants across the program
* namespace - types : defines uuid and username types
* enums - RequestCode, ResponseCode, MessageType, UserCommand : used for type assigning for messages, user commands, requests, and responses
* 
*/

#include <cstdint>
#include <iostream>
#include <array>
#include <map>
#include <string>

/*
* Represents a list of the different pre-defined request codes
*/
enum class RequestCode : uint16_t {
	REGISTRATION_REQUEST = 600,
	CLIENT_LIST_REQUEST = 601,
	PUBLIC_KEY_REQUEST = 602,
	MESSAGE_SEND_REQUEST = 603,
	AWAITING_MESSAGES_REQUEST = 604,
	INVALID_REQUEST_CODE = -1,
	EXIT_REQUEST = 0
};

/*
* Represents a list of the different pre-defined response codes
*/
enum class ResponseCode : uint16_t {
	SUCCESSFUL_REGISTRATION = 2100,
	CLIENT_LIST = 2101,
	PUBLIC_KEY = 2102,
	SUCCESSFUL_CLIENT_MESSAGE = 2103,
	AWAITING_MESSAGES = 2104,
	GENERAL_ERROR = 9000
};

/*
* Represents a list of the different pre-defined message types
*/
enum class MessageType : uint8_t {
	SYMMETRIC_KEY_REQUEST_TYPE = 1,
	SYMMETRIC_KEY_SEND_TYPE = 2,
	MESSAGE_SEND_TYPE = 3
};

/*
* Represents a list of the different pre-defined user commands
*/
enum class UserCommand : uint8_t {
	REGISTRATION = 110,
	CLIENT_LIST = 120,
	PUBLIC_KEY_REQUEST = 130,
	WAITING_MESSAGES = 140,
	SEND_TXT_MSG = 150,
	REQUEST_SYMMETRIC_KEY = 151,
	SEND_SYMMETRIC_KEY = 152,
	EXIT = 0,
	INVALID_COMMAND = -1
};

namespace constants {
	const std::string server_info = "server.info";
	const std::string my_info = "my.info";

	constexpr uint8_t CLIENT_VERSION = 1;
	constexpr size_t UUID_LENGTH = 16; 
	constexpr size_t USERNAME_LENGTH = 255;
	constexpr size_t PUBLIC_KEY_LENGTH = 160;
	constexpr size_t VERSION_LENGTH = 1;
	constexpr size_t PAYLOAD_SIZE_LENGTH = 4;
	constexpr size_t CODE_LENGTH = 2;
	constexpr size_t MESSAGE_TYPE_LENGTH = 1;
	constexpr size_t CONTENT_SIZE_LENGTH = 4;
	constexpr size_t MESSAGE_ID_LENGTH = 4;
	constexpr size_t MESSAGE_SIZE_LENGTH = 4;
	constexpr size_t EMPTY_PAYLOAD = 0;
	constexpr size_t FILE_HEX_SIZE = 2;

	constexpr size_t REQUEST_HEADER_LENGTH = UUID_LENGTH + VERSION_LENGTH + CODE_LENGTH + PAYLOAD_SIZE_LENGTH;
	constexpr size_t RESPONSE_HEADER_LENGTH = VERSION_LENGTH + CODE_LENGTH + PAYLOAD_SIZE_LENGTH;

	// maps the request code to the size of the request data -> MESSAGE_SEND_REQUEST needs to have the content of the message accounted to as it is a variable and cant be pre-calculated
	static const std::map<RequestCode, size_t> REQUEST_SIZE_MAP =
	{
		{RequestCode::REGISTRATION_REQUEST, REQUEST_HEADER_LENGTH + USERNAME_LENGTH + PUBLIC_KEY_LENGTH},
		{RequestCode::CLIENT_LIST_REQUEST, REQUEST_HEADER_LENGTH},
		{RequestCode::PUBLIC_KEY_REQUEST, REQUEST_HEADER_LENGTH + UUID_LENGTH},
		{RequestCode::MESSAGE_SEND_REQUEST, REQUEST_HEADER_LENGTH + UUID_LENGTH + MESSAGE_TYPE_LENGTH + CONTENT_SIZE_LENGTH}, // needs to have the size of the actual content added to it when used
		{RequestCode::AWAITING_MESSAGES_REQUEST, REQUEST_HEADER_LENGTH}
	};

	// maps the response code to the size of the response data -> CLIENT_SIZE, AWAITING_MESSAGES needs to have the content of the response accounted to as it is a variable and cant be pre-calculated
	static const std::map<ResponseCode, size_t> RESPONSE_SIZE_MAP =
	{
		{ResponseCode::SUCCESSFUL_REGISTRATION, RESPONSE_HEADER_LENGTH + UUID_LENGTH},
		{ResponseCode::CLIENT_LIST, RESPONSE_HEADER_LENGTH}, // needs to have payload_size added to it
		{ResponseCode::PUBLIC_KEY, RESPONSE_HEADER_LENGTH + UUID_LENGTH + PUBLIC_KEY_LENGTH},
		{ResponseCode::SUCCESSFUL_CLIENT_MESSAGE, RESPONSE_HEADER_LENGTH + UUID_LENGTH + MESSAGE_ID_LENGTH},
		{ResponseCode::AWAITING_MESSAGES, RESPONSE_HEADER_LENGTH + UUID_LENGTH + MESSAGE_ID_LENGTH + MESSAGE_TYPE_LENGTH + MESSAGE_SIZE_LENGTH}// needs to have message_size added to it
	};

	//maps UserCommand to the MessageType
	static const std::map<UserCommand, MessageType> COMMAND_MESSAGE_TYPE_MAP =
	{
		{UserCommand::SEND_TXT_MSG, MessageType::MESSAGE_SEND_TYPE},
		{UserCommand::REQUEST_SYMMETRIC_KEY, MessageType::SYMMETRIC_KEY_REQUEST_TYPE},
		{UserCommand::SEND_SYMMETRIC_KEY, MessageType::SYMMETRIC_KEY_SEND_TYPE}
	};
}

/*
* defines uuid and username types
*/
namespace types {
	typedef std::array<uint8_t, constants::UUID_LENGTH> uuid;
	typedef std::array<uint8_t, constants::USERNAME_LENGTH> username;
}