#pragma once
/**
* @author Ilay Zvi
* @file Message.h
* contents: 
* class - Message : represents a user message to be sent to the server(and later to the client)
*
*/
#include "Definitions.h"

class Message {
private:
	MessageType msg_type;
	uint32_t content_size;
	std::string message_content;
public:
	explicit Message(MessageType msg_type, uint32_t content_size);

	explicit Message(MessageType type, uint32_t content_size, std::string message_content);

	/*
	* @returns the message's MessageType
	*/
	MessageType get_msg_type() const;

	/*
	* @returns the size of the message content
	*/
	uint32_t get_content_size() const;

	/*
	* @returns the content of the message
	*/
	std::string get_message_content() const;
};