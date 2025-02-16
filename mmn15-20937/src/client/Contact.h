#pragma once
/**
* @author Ilay Zvi
* @file Contact.h
* contents:
* class - Contact : represents a contact in the client's list of contacts
*
*/
#include "Definitions.h"
#include <string>
#include "AESWrapper.h"
#include <stdexcept>

class Contact {
private:
	types::username username;
	types::uuid contact_id;
	std::string rsa_pub, symmetric_key;
public:
	
	/*
	* initializes an object of type Contact
	* throws invalid argument exception when given a key that is of improper size
	*/
	explicit Contact(types::username username, types::uuid contact_id);

	/*
	* @returns the contact's username
	*/
	types::username get_username() const;

	/*
	* @returns the contact's id
	*/
	types::uuid get_contact_id() const;

	/*
	* @returns the contact's public key
	*/
	std::string get_rsa_pub() const;

	/*
	* @returns the contact's symmetric key
	*/
	std::string get_symmetric_key() const;

	/*
	* sets public key
	*/
	void set_rsa_pub(std::string rsa_pub);

	/*
	* sets symmetric key
	*/
	void set_symmetric_key(std::string symmetric_key);
};