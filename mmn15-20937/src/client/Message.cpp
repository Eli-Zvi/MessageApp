#include "Message.h"

Message::Message(MessageType msg_type, uint32_t content_size) {
	this->msg_type = msg_type;
	this->content_size = content_size;
	this->message_content = "";
}

Message::Message(MessageType msg_type, uint32_t content_size, std::string message_content)
{
	this->msg_type = msg_type;
	this->content_size = content_size;
	this->message_content = message_content;
}

MessageType Message::get_msg_type() const
{
	return this->msg_type;
}

uint32_t Message::get_content_size() const
{
	return this->content_size;
}

std::string Message::get_message_content() const
{
	return this->message_content;
}