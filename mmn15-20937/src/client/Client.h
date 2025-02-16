#pragma once
/**
* @author Ilay Zvi
* @file Client.h
* contents:
* class - Client: represents this program's Client in the MessageU messaging app
*
*/

#include <iostream>
#include <boost/asio.hpp>
#include <map>
#include <string>
#include "Contact.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>
#include "UserIO.h"

class Client {
private:
	types::uuid client_id;
	types::username username;
	std::string pub_key, priv_key;
	std::map<types::username, Contact> contact_list;
	std::map <types::uuid, types::username> uuid_username_list;
	bool _is_null = false;
public:
	Client(types::uuid client_id, types::username username, std::string public_key, std::string private_key);

	/*
	* attempts to register using my.info otherwise creates null instance
	* null instance will be used until user registers
	*
	*/
	Client();

	const types::uuid get_client_id() const;

	const types::username get_username() const;

	const std::string get_pub_key() const;

	const std::string get_private_key() const;

	const bool is_null() const;

	/*
	* @param username to get the contact info of
	* @return the Contact object
	* throws std::out_of_range exception
	*/
	Contact& get_contact(types::username username);

	/*
	* @param Contact - the contact to add to contact list
	*/
	void add_contact(Contact& c);

	/*
	* @param uuid - id to convert to username
	* @returns username related to the id
	*/
	types::username username_from_uuid(types::uuid id);

	/*
	* @param username - username to be checked
	* @returns true if username exists in contact list
	*/
	bool is_username_exist(types::username username);
};