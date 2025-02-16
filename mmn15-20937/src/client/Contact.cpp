#include "Contact.h"


Contact::Contact(types::username username, types::uuid contact_id)
{
	this->username = username;
	this->contact_id = contact_id;
	this->rsa_pub = "";
	this->symmetric_key = "";
}

types::username Contact::get_username() const
{
	return this->username;
}

types::uuid Contact::get_contact_id() const
{
	return this->contact_id;
}

std::string Contact::get_rsa_pub() const{
	return this->rsa_pub;
}

std::string Contact::get_symmetric_key() const
{
	return this->symmetric_key;
}

void Contact::set_rsa_pub(std::string rsa_pub)
{
	if(rsa_pub.size() == constants::PUBLIC_KEY_LENGTH)
		this->rsa_pub = rsa_pub;
	else throw std::out_of_range("Invalid rsa public key length");
}

void Contact::set_symmetric_key(std::string symmetric_key)
{
	if (symmetric_key.size() == AESWrapper::DEFAULT_KEYLENGTH)
		this->symmetric_key = symmetric_key;
	else throw std::out_of_range("Invalid symmetric key length");
}