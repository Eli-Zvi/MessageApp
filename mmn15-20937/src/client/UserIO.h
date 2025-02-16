#pragma once

/**
* @author Ilay Zvi
* @file UserIO.h
* contents:
* namespace : UserIO - defines a namespace with user input/output methods used by other namespaces and classes in the project
*	
*/
#include <iostream>
#include <boost/asio.hpp>
#include "Definitions.h"
#include <string>
#include "Message.h"
#include "exceptions.h"
#include <iomanip>
#include "Contact.h"
#include "Encryption.h"
#include "RSAWrapper.h"

namespace UserIO {

	/*
	* prompts user to pick a request/command type
	* @returns UserCommand
	*/
	UserCommand get_user_input();

	/*
	* validates that user gave a proper UserCommand value
	* @returns UserCommand
	*/
	static UserCommand validate_string(int input);
	
	/*
	* gets username for requests that require username
	* @return a types::username
	*/
	types::username get_username_input(UserCommand code);

	/*
	* Returns one of four options
	* 1. a request for a symmetric key by the user with empty message content
	* 2. a message to another user with a newly created symmetric key that is encrypted using the dest user's rsa pub key
	* 3. a message to another user encrypted with the symmetrical key shared by the both of them
	* 4. an invalid argument error when the message type does not exist
	*/
	Message get_message_input(types::uuid id, MessageType type, Contact& c);
}