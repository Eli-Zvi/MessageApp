#include "Client.h"

Client::Client(types::uuid client_id, types::username username, std::string public_key, std::string private_key)
{
    this->client_id = client_id;
    this->username = username;
    this->pub_key = public_key;
    this->priv_key = private_key;
}


Client::Client()
{
    this->pub_key = "";
    
    try {
        std::stringstream buffer;
        std::string uname_string, uid_string, private_key, line;
        types::uuid uid = { 0 };
        types::username uname = { 0 };
        
        if (!std::filesystem::exists(constants::my_info)) {
            throw std::exception(); // if the file doesnt exist go to default initialization 
        }

        std::ifstream file(constants::my_info);

        if (!file.is_open()) {
            throw std::exception(); //if the file cant be opened go to default initialization
        }
        
        buffer << file.rdbuf(); //read entire my_info file
        file.close();
        
        std::getline(buffer, uname_string, '\n'); //get username from file
        std::getline(buffer, uid_string, '\n'); //get uuid from file

        if (uname_string.size() > constants::USERNAME_LENGTH - 1 || uid_string.size() != constants::UUID_LENGTH * constants::FILE_HEX_SIZE) {
            
            throw std::exception();
        }

        for (size_t i = 0; i < uname_string.size(); i++) {
            uname[i] = static_cast<uint8_t>(uname_string[i]); //copy username
        }

        for (size_t i = 0; i < uid.size(); i++) {
            if (std::isxdigit(uid_string[i * constants::FILE_HEX_SIZE]) && std::isxdigit(uid_string[i * constants::FILE_HEX_SIZE + 1])) {
                uid[i] = static_cast<uint8_t>(std::stoi(uid_string.substr(i * constants::FILE_HEX_SIZE, constants::FILE_HEX_SIZE), nullptr, 16)); //parse uuid
            }
            else {
                throw std::exception();
            }
        }
        
        while (std::getline(buffer, line)) {
            private_key += line;
        }

        this->username = uname;
        this->client_id = uid;
        this->priv_key = private_key;
        std::cout << "Signed in with username: ";
        for (size_t i = 0; i < uname.size(); i++)
            std::cout << uname[i];
        std::cout << std::endl;
    }
    catch (const std::exception& e) {
        this->client_id = { 0 };
        this->username = { 0 };

        this->_is_null = true;
    }
}

const types::uuid Client::get_client_id() const
{
    return this->client_id;
}

const types::username Client::get_username() const
{
    return this->username;
}

const std::string Client::get_pub_key() const
{
    return this->pub_key;
}

const std::string Client::get_private_key() const
{
    return this->priv_key;
}

const bool Client::is_null() const
{
    return this->_is_null;
}


Contact& Client::get_contact(types::username username)
{
    return this->contact_list.at(username); //return username from contact_list if it exists
}

void Client::add_contact(Contact& c)
{
    this->contact_list.insert_or_assign(c.get_username(), c ); //assign/insert to contact_list
    this->uuid_username_list.insert_or_assign(c.get_contact_id(), c.get_username()); //assign/insert to uuid -> username list
}

types::username Client::username_from_uuid(types::uuid id)
{
    return this->uuid_username_list.at(id); //check if id exists in uuid_username_list and return it
}

bool Client::is_username_exist(types::username username) {
    return this->contact_list.count(username) > 0; //check if username exists in contact_list and return true or false accordingly
}