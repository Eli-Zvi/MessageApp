#include "RequestHandler.h"


void RequestHandler::call_handler() {
    try {
        boost::asio::io_context io_context;

        // establish connect
        tcp::resolver resolver(io_context);
        std::ifstream address_file(constants::server_info);

        if (!address_file.is_open()) {
            std::cerr << "File server.info can't be opened registration failed\n";
            throw std::runtime_error("");
        }

        if (address_file.peek() == std::ifstream::traits_type::eof()) {
            std::cerr << "File server.info is empty\n";
            throw std::runtime_error("");
        }

        std::string temp, host, port;
        
        std::getline(address_file, temp);
        
        std::stringstream address(temp);

        std::getline(address, host, ':');
        std::getline(address, port);

        auto endpoints = resolver.resolve(host, port);

        // Create a socket
        tcp::socket socket(io_context);

        // Connect to the server
        boost::asio::connect(socket, endpoints);

        Client c;

        while (true) 
        {
            try 
            {
                UserCommand code = UserIO::get_user_input();

                if (code == UserCommand::REGISTRATION) 
                {
                    if (c.is_null()) 
                    {
                        c = registration_handler(UserIO::get_username_input(code), socket);
                    }
                    else std::cout << "Already registered" << std::endl;
                    
                }
                else if (code == UserCommand::EXIT) 
                {
                    socket.close();
                    exit(0); // nothing to destruct - no dynamic memory across the program
                }
                else if(code == UserCommand::INVALID_COMMAND)
                {
                    std::cout << "Invalid Request Code" << std::endl;

                }
                else 
                {
                    if (c.is_null()) 
                    {
                        std::cout << "Not registered yet" << std::endl;
                    }
                    else 
                    {
                        switch (code) {
                        case UserCommand::CLIENT_LIST:
                            client_list_handler(c, socket);
                            break;
                        case UserCommand::PUBLIC_KEY_REQUEST:
                            pub_key_handler(c, socket);
                            break;
                        case UserCommand::SEND_SYMMETRIC_KEY: 
                        case UserCommand::SEND_TXT_MSG:
                        case UserCommand::REQUEST_SYMMETRIC_KEY:
                            message_send_handler(c, socket, constants::COMMAND_MESSAGE_TYPE_MAP.at(code));
                            break;
                        case UserCommand::WAITING_MESSAGES:
                            awaiting_message_handler(c, socket);
                        }
                    }
                }
            }
            catch (const boost::system::system_error& e) 
            {
                std::cerr << "socket error:\n" << std::endl;
                exit(0); // issues with server communication could cause undefined behavior - better to disconnect
            }
            catch (const GeneralError& e) 
            {
                std::cerr << e.what() << std::endl;
            }
            catch (const UserError& e) 
            {
                std::cerr << e.what() << std::endl;
            }
            catch (const std::exception& e) 
            {
                std::cerr << "An Unexpected Error has occured:\n" << e.what() << std::endl;
            }
        }
    }
    catch (const boost::system::system_error& e) {
        std::cerr << "error while establishing connection\n";
    }
    catch (const std::exception& e) {
        std::cerr << "An Unexpected Error has occured:\n";
    }
    
}


Client RequestHandler::registration_handler(types::username name, tcp::socket& sock)
{
    std::ofstream file(constants::my_info, std::ofstream::out); //open user info file

    if (!file.is_open()) {
        throw std::runtime_error("File my.info can't be opened registration failed");
    }

    std::vector<unsigned char> request_buffer(constants::REQUEST_SIZE_MAP.at(RequestCode::REGISTRATION_REQUEST)); //create buffer for request
    build_header(request_buffer, {0}, RequestCode::REGISTRATION_REQUEST, constants::USERNAME_LENGTH + constants::PUBLIC_KEY_LENGTH); //build the header for the request

    size_t offset = constants::REQUEST_HEADER_LENGTH;

    RSAPrivateWrapper rsapriv; //initialize rsa key pair

    std::string pub_key = rsapriv.getPublicKey(); //size 160 public rsa key
    std::string base64key = Base64Wrapper::encode(rsapriv.getPrivateKey()); //get private key
 
    //copy payload to buffer
    std::copy(name.begin(), name.end(), request_buffer.data() + offset); 
    offset += constants::USERNAME_LENGTH;

    std::copy(pub_key.begin(), pub_key.end(), request_buffer.data() + offset);

    ServerCommunication::write_buffer(sock, request_buffer); //send to server

    types::uuid uid = { 0 };

    std::tuple<ResponseCode, uint32_t> responseHeader = parse_response_header(sock);

    if (std::get<0>(responseHeader) != ResponseCode::SUCCESSFUL_REGISTRATION) //validate response code
        throw std::runtime_error("Invalid response from server");

    uint32_t payload_size = std::get<1>(responseHeader); //get payload size

    std::vector<unsigned char> payload = parse_response_payload(sock, payload_size);

    std::memcpy(&uid, payload.data(), payload_size);

    //writes username, uuid, priv key
    file.write(reinterpret_cast<const char*>(name.data()), constants::USERNAME_LENGTH - 1); //write username to file
    file << '\n';

    file << std::hex;

    for (size_t i = 0; i < uid.size(); i++) {
        file << std::setfill('0') << std::setw(constants::FILE_HEX_SIZE) << static_cast<unsigned int>(uid[i]); //hexifies uuid and writes to file
    }
    file << '\n';
    
    file << std::dec; //reset to default - possibly remove

    file.write(base64key.c_str(), base64key.size()); //write private key to file

    //check that file is good
    if (!file.good()) {
        if (std::filesystem::exists(constants::my_info)) {
            std::filesystem::remove(constants::my_info);
            std::cerr << "Failure while writing to my.info, my.info will be deleted" << std::endl;
        }
    }
    
    file.close();
    std::cout << "successfully registered" << std::endl;
    return Client(uid, name, pub_key, base64key);
}


void RequestHandler::client_list_handler(Client& c, tcp::socket& sock) {
    std::vector<unsigned char> request_buffer(constants::REQUEST_SIZE_MAP.at(RequestCode::CLIENT_LIST_REQUEST)); //initialize buffer for request
    build_header(request_buffer, c.get_client_id(), RequestCode::CLIENT_LIST_REQUEST, constants::EMPTY_PAYLOAD); //build the header for the request

    ServerCommunication::write_buffer(sock, request_buffer); // send request to server

    std::tuple<ResponseCode, uint32_t> responseHeader = parse_response_header(sock); //get response from server

    if (std::get<0>(responseHeader) != ResponseCode::CLIENT_LIST) //validate response code
        throw std::runtime_error("Invalid response from server");

    uint32_t payload_size = std::get<1>(responseHeader); //get payload size

    std::vector<unsigned char> payload = parse_response_payload(sock, std::get<1>(responseHeader)); //get payload

    size_t client_record_size = constants::UUID_LENGTH + constants::USERNAME_LENGTH; //size of a client record
    size_t number_of_users = payload_size / client_record_size; // get total number of clients in the list

    for (size_t client_number = 0; client_number < number_of_users; client_number++) {
        types::uuid id = { 0 };
        std::memcpy(&id, payload.data() + (client_number * client_record_size), constants::UUID_LENGTH);
        types::username uname = { 0 };
        std::memcpy(&uname, payload.data() + (client_number * client_record_size) + constants::UUID_LENGTH, constants::USERNAME_LENGTH);
        std::cout << std::string(uname.begin(), uname.end()) << std::endl;
        Contact temp(uname, id);
        c.add_contact(temp); //add new contact
    }

}


void RequestHandler::pub_key_handler(Client& c, tcp::socket& sock) {
    types::username contact_username = UserIO::get_username_input(UserCommand::PUBLIC_KEY_REQUEST); //get the contact's username from user

    if (!c.is_username_exist(contact_username)) //check if it exists
        throw UserError("Username does not exist");

    std::vector<unsigned char> request_buffer(constants::REQUEST_SIZE_MAP.at(RequestCode::PUBLIC_KEY_REQUEST)); //create buffer for request
    build_header(request_buffer, c.get_client_id(), RequestCode::PUBLIC_KEY_REQUEST, constants::UUID_LENGTH); //create header for request

    Contact& contact = c.get_contact(contact_username);
    types::uuid contact_id = contact.get_contact_id(); 

    std::copy(contact_id.begin(), contact_id.end(), request_buffer.data() + constants::REQUEST_HEADER_LENGTH); //copy id into payload

    ServerCommunication::write_buffer(sock, request_buffer);

    std::tuple<ResponseCode, uint32_t> responseHeader = parse_response_header(sock);

    if (std::get<0>(responseHeader) != ResponseCode::PUBLIC_KEY) //validate response code
        throw std::runtime_error("Invalid response from server");

    uint32_t payload_size = std::get<1>(responseHeader);

    std::vector<unsigned char> payload = parse_response_payload(sock, payload_size);

    types::uuid temp_id = { 0 };

    std::memcpy(&temp_id, payload.data(), constants::UUID_LENGTH);

    size_t offset = constants::UUID_LENGTH;

    if (std::memcmp(temp_id.data(), contact_id.data(), constants::UUID_LENGTH) != 0) {
        std::cout << "Given wrong uid by server";
        return;
    }

    std::string rsa_pub_key(constants::PUBLIC_KEY_LENGTH, '\0');

    std::memcpy(rsa_pub_key.data(), payload.data() + offset, constants::PUBLIC_KEY_LENGTH);

    contact.set_rsa_pub(rsa_pub_key);
}


void RequestHandler::message_send_handler(Client& c, tcp::socket& sock, MessageType type) {
    types::username contact_username = UserIO::get_username_input(UserCommand::SEND_TXT_MSG); // prompt user for username of destination

    if (!c.is_username_exist(contact_username)) //check that the username is in the contact list of the user
        throw UserError("Username does not exist");

    Contact& dest_contact = c.get_contact(contact_username); //get the contact information of the destination user
    types::uuid dest_uid = dest_contact.get_contact_id(); //get the uuid of the destination user

    Message msg = UserIO::get_message_input(dest_uid, type, dest_contact); //create a message

    std::vector<unsigned char> request_buffer(constants::REQUEST_SIZE_MAP.at(RequestCode::MESSAGE_SEND_REQUEST) + msg.get_content_size());
    build_header(request_buffer, c.get_client_id(), RequestCode::MESSAGE_SEND_REQUEST, 
        msg.get_content_size() + constants::UUID_LENGTH + constants::MESSAGE_TYPE_LENGTH + constants::CONTENT_SIZE_LENGTH);

    size_t offset = constants::REQUEST_HEADER_LENGTH;

    std::copy(dest_uid.begin(), dest_uid.end(), request_buffer.data() + offset); //add uuid to buffer
    offset += constants::UUID_LENGTH;

    uint8_t msg_type = static_cast<uint8_t>(msg.get_msg_type()); 
    std::memcpy(request_buffer.data() + offset, &msg_type, constants::MESSAGE_TYPE_LENGTH);// add msg type to buffer
    offset += constants::MESSAGE_TYPE_LENGTH;

    uint32_t le_content_size = boost::endian::native_to_little(msg.get_content_size()); 
    std::memcpy(request_buffer.data() + offset, &le_content_size, constants::MESSAGE_SIZE_LENGTH);//add content size to buffer
    offset += constants::MESSAGE_SIZE_LENGTH;

    if(msg.get_msg_type() != MessageType::SYMMETRIC_KEY_REQUEST_TYPE) //if not symmetric request, there is a payload to the msg
        std::memcpy(request_buffer.data() + offset, msg.get_message_content().data(), msg.get_content_size()); //add content to buffer

    ServerCommunication::write_buffer(sock, request_buffer);

    std::tuple<ResponseCode, uint32_t> responseHeader = parse_response_header(sock);

    if (std::get<0>(responseHeader) != ResponseCode::SUCCESSFUL_CLIENT_MESSAGE) //validate response code
        throw std::runtime_error("Invalid response from server");

    uint32_t payload_size = std::get<1>(responseHeader);

    std::vector<unsigned char> payload = parse_response_payload(sock, payload_size);

    types::uuid temp_id = { 0 };

    std::memcpy(&temp_id, payload.data(), constants::UUID_LENGTH);

    offset = constants::UUID_LENGTH;

    if (std::memcmp(temp_id.data(), dest_uid.data(), constants::UUID_LENGTH) != 0) {
        std::cout << "Given wrong uid by server";
        return;
    }

    uint32_t message_id;

    std::memcpy(&message_id, payload.data() + offset, sizeof(message_id));

    std::cout << "Server has received the message successfully with the id: " << message_id << std::endl;
}


void RequestHandler::awaiting_message_handler(Client& c, tcp::socket& sock) {
    std::vector<unsigned char> request_buffer(constants::REQUEST_SIZE_MAP.at(RequestCode::AWAITING_MESSAGES_REQUEST));
    build_header(request_buffer, c.get_client_id(), RequestCode::AWAITING_MESSAGES_REQUEST, constants::EMPTY_PAYLOAD);

    ServerCommunication::write_buffer(sock, request_buffer);

    std::tuple<ResponseCode, uint32_t> responseHeader = parse_response_header(sock);

    if (std::get<0>(responseHeader) != ResponseCode::AWAITING_MESSAGES) //validate response code
        throw std::runtime_error("Invalid response from server");

    uint32_t payload_size = std::get<1>(responseHeader);

    std::vector<unsigned char> payload = parse_response_payload(sock, payload_size);

    size_t offset = 0;

    while (offset < payload_size) {

        std::string output_string = "From: ";
        types::uuid temp_id = { 0 };
        types::username sender_name = { 0 };

        std::memcpy(&temp_id, payload.data(), constants::UUID_LENGTH);
        offset += constants::UUID_LENGTH;

        try {
            uint32_t message_id;

            std::memcpy(&message_id, payload.data() + offset, sizeof(message_id));
            message_id = boost::endian::little_to_native(message_id); //get message id
            offset += constants::MESSAGE_ID_LENGTH;

            uint8_t temp; // stores the message type value
            std::memcpy(&temp, payload.data() + offset, sizeof(temp)); //get message type
            MessageType msg_type = MessageType(temp);
            offset += constants::MESSAGE_TYPE_LENGTH;

            uint32_t msg_size;
            std::memcpy(&msg_size, payload.data() + offset, sizeof(msg_size)); //get message size
            msg_size = boost::endian::little_to_native(msg_size);
            offset += constants::MESSAGE_SIZE_LENGTH;

            std::string content(reinterpret_cast<const char*>(payload.data() + offset), msg_size); //get message contents
            offset += msg_size;

            sender_name = c.username_from_uuid(temp_id);

            output_string += std::string(sender_name.begin(), sender_name.end()) += "\n";

            std::cout << output_string << "Content:\n" 
                << parse_message_content(content, msg_type, c.get_contact(sender_name), c) << std::endl << "----<EOM>----" << std::endl; //attempt to decrypt and display message
        }
        catch (std::out_of_range& e) {
            std::cout << ("Message sent by a nameless user") << std::endl; //if user was not registered in contact list
        }
    }
}

std::string RequestHandler::parse_message_content(std::string content, MessageType type, Contact& sender, Client& c) {
    if (type == MessageType::SYMMETRIC_KEY_REQUEST_TYPE) {
        return "Request for symmetric key";
    }
    else if (type == MessageType::SYMMETRIC_KEY_SEND_TYPE) {
        RSAPrivateWrapper rsapriv(Base64Wrapper::decode(c.get_private_key())); //get private key

        sender.set_symmetric_key(rsapriv.decrypt(content)); //decrypt message

        return "symmetric key received";
    }
    else if(type == MessageType::MESSAGE_SEND_TYPE){
        std::string symmetrical_key = sender.get_symmetric_key(); //get symmetric key

        if (symmetrical_key == "") {
            return "can't decrypt message";
        }

        unsigned char key[AESWrapper::DEFAULT_KEYLENGTH];
        std::copy(symmetrical_key.begin(), symmetrical_key.end(), key); //copy key into unsigned char

        AESWrapper aes(key, AESWrapper::DEFAULT_KEYLENGTH); //create aes key

        std::string plaintext = aes.decrypt(content.c_str(), content.length()); //encrypt user message

        Encryption::erase_key(key, AESWrapper::DEFAULT_KEYLENGTH); //erase aes key

        return plaintext;
    }
    
    return "unknown message type";
}


void RequestHandler::build_header(std::vector<unsigned char>& buffer, const types::uuid client_id, RequestCode code, size_t payload_size) {
    size_t offset = 0;
    uint16_t le_code = boost::endian::native_to_little(static_cast<uint16_t>(code)); //convert requestcode to little endian
    uint32_t le_payload_size = boost::endian::native_to_little(payload_size); //convert payload size to little endian

    auto copy_data = [&](const void* data, size_t size) {
        std::memcpy(buffer.data() + offset, data, size); //copy data to buffer
        offset += size;
    };

    copy_data(&client_id, constants::UUID_LENGTH); //already little endian

    copy_data(&constants::CLIENT_VERSION, constants::VERSION_LENGTH); //already little endian

    copy_data(&le_code, constants::CODE_LENGTH);
    
    copy_data(&payload_size, constants::PAYLOAD_SIZE_LENGTH);
}


std::tuple<ResponseCode, uint32_t> RequestHandler::parse_response_header(tcp::socket& sock) {
    size_t offset = 0;
    std::vector<unsigned char> buffer(constants::RESPONSE_HEADER_LENGTH);

    if (ServerCommunication::read_buffer(sock, buffer, constants::RESPONSE_HEADER_LENGTH) != constants::RESPONSE_HEADER_LENGTH) //check if header is correct size
        throw std::runtime_error("Invalid header size");

    uint8_t version;
    uint16_t _code;
    uint32_t payload_size;

    //copy version, response code and payload size
    auto copy_data = [&](void* data, size_t size) {
        std::memcpy(data, buffer.data() + offset, size); 
        offset += size;
        };

    copy_data(&version, constants::VERSION_LENGTH);
    copy_data(&_code, constants::CODE_LENGTH);
    copy_data(&payload_size, constants::PAYLOAD_SIZE_LENGTH);

    ResponseCode code = static_cast<ResponseCode>(boost::endian::little_to_native(_code)); //get response code

    if (code == ResponseCode::GENERAL_ERROR) //check if server responded with an error
        throw GeneralError();
    
    return std::tuple(code, boost::endian::little_to_native(payload_size)); //return a tuple containing the response code and the payload size in little endian
}

std::vector<unsigned char> RequestHandler::parse_response_payload(tcp::socket& sock, size_t buffer_size) {
    std::vector<unsigned char> buffer(buffer_size);
    
    if (ServerCommunication::read_buffer(sock, buffer, buffer_size) != buffer_size)
        throw std::runtime_error("Invalid payload size");

    return buffer;
}